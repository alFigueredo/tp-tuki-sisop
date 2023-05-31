#include "cliente.h"

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,
            server_info->ai_socktype,
            server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		log_error(logger, "¡No se pudo conectar al servidor!");
		abort();
	}
	freeaddrinfo(server_info);
	send_handshake(socket_cliente);
	log_info(logger, "¡Conectado al servidor!");
	return socket_cliente;
}

void send_handshake(int socket_cliente)
{
	uint32_t handshake = 1;
	uint32_t result;
	send(socket_cliente, &handshake, sizeof(uint32_t), 0);
	if (recv(socket_cliente, &result, sizeof(uint32_t), MSG_WAITALL) == -1 || result == -1) {
		log_error(logger, "¡Protocolo incompatible con el servidor!");
		abort();
	}
}

void esperar_servidor(int conexion){
	pthread_t thread;
	int *socket_servidor = malloc(sizeof(int));
	*socket_servidor = conexion;
	pthread_create(&thread,
	               NULL,
	               (void*) atender_servidor,
	               socket_servidor);
	pthread_detach(thread);
}

void atender_servidor(int* socket_servidor){
	t_list *lista;
	pthread_t thread;
	char* instruccion;
	while (1) {
		int cod_op = recibir_operacion(*socket_servidor);
		switch (cod_op) {
			case MENSAJE:
				recibir_mensaje(*socket_servidor);
				break;
			case PAQUETE:
				lista = recibir_paquete(*socket_servidor);
				log_info(logger, "Me llegaron los siguientes valores:");
				list_iterate(lista, (void*) iterator);
				list_clean_and_destroy_elements(lista, free);
				break;
			case READY:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				exec_a_ready();
				break;
			case IO_BLOCK:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				pthread_create(&thread, NULL, (void*) io_block, NULL);
				pthread_detach(thread);
				break;
			case WAIT:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				log_trace(logger, "PID: %d - Wait", ((pcb*)queue_peek(qexec))->pid);
				log_trace(logger, "PID. %d - Instruccion: %s", ((pcb*)queue_peek(qexec))->pid, instruccion);
				enviar_pcb(conexion_cpu, queue_peek(qexec), EXEC);
				break;
			case SIGNAL:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				log_trace(logger, "PID: %d - Signal", ((pcb*)queue_peek(qexec))->pid);
				log_trace(logger, "PID. %d - Instruccion: %s", ((pcb*)queue_peek(qexec))->pid, instruccion);
				enviar_pcb(conexion_cpu, queue_peek(qexec), EXEC);
				break;
			case EXIT:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				exec_a_exit();
				break;
			case -1:
				log_warning(logger, "El servidor se desconecto. Terminando conexion. Abortando sistema.");
				free(socket_servidor);
				abort();
				return;
			default:
				log_warning(logger,"Operacion desconocida. No quieras meter la pata");
				break;
		}
	}
}

void liberar_conexion(int socket_cliente)
{
	if (socket_cliente != -1) {
	log_warning(logger, "Liberando conexion");
	close(socket_cliente);
	}
}
