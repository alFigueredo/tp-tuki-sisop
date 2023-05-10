#include "cliente.h"

int conexion_cpu;
int conexion_memoria;
int conexion_filesystem;

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
	pcb* proceso;
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
				break;
			case READY:
				lista = recibir_paquete(*socket_servidor);
				proceso = recibir_pcb_de_cpu(lista);
				queue_pop(qexec);
				queue_push(qready, proceso);
				log_info(logger, "El proceso %d pasa a READY", proceso->pid);
				queue_push(qexec, queue_pop(qready));
				log_info(logger, "El proceso %d pasa a EXEC", proceso->pid);
				enviar_pcb_a_cpu(conexion_cpu, queue_peek(qexec));
				break;
			case FINISHED:
				lista = recibir_paquete(*socket_servidor);
				proceso = recibir_pcb_de_cpu(lista);
				queue_pop(qexec);
				queue_push(qexit, proceso);
				log_info(logger, "El proceso %d pasa a EXIT", proceso->pid);
				break;
			case -1:
				log_warning(logger, "El servidor se desconecto. Terminando conexion");
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
