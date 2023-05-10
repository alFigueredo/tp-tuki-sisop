#include "server.h"

int iniciar_servidor(char* puerto)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo;
	// struct addrinfo *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo);

	// Creamos el socket de escucha del servidor

	socket_servidor = socket(servinfo->ai_family,
	                         servinfo->ai_socktype,
	                         servinfo->ai_protocol);

	// Asociamos el socket a un puerto

	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes

	if (listen(socket_servidor, SOMAXCONN) == -1) {
		log_error(logger, "¡No se pudo iniciar el servidor!");
		exit(1);
	}

	freeaddrinfo(servinfo);
	log_info(logger, "Servidor listo para recibir al cliente");

	return socket_servidor;
}

void esperar_cliente(int socket_servidor){
	while (1) {
	   pthread_t thread;
	   int *socket_cliente = malloc(sizeof(int));
	   *socket_cliente = accept(socket_servidor, NULL, NULL);
	   log_info(logger, "¡Se conecto un cliente!");
	   recv_handshake(*socket_cliente);
	   pthread_create(&thread,
	                  NULL,
	                  (void*) atender_cliente,
	                  socket_cliente);
	   pthread_detach(thread);
	}
}

void atender_cliente(int* socket_cliente){
	t_list *lista;
	pcb* proceso;
	while (1) {
		int cod_op = recibir_operacion(*socket_cliente);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(*socket_cliente);
			break;
		case PAQUETE:
			lista = recibir_paquete(*socket_cliente);
			log_info(logger, "Me llegaron los siguientes valores:");
			list_iterate(proceso->instrucciones, (void*) iterator);
			break;
		case EXEC:
			lista = recibir_paquete(*socket_cliente);
			proceso = recibir_pcb_de_kernel(lista);
			switch ((int)interpretar_instrucciones(proceso)) {
				case YIELD:
					enviar_pcb_a_kernel(*socket_cliente, proceso, READY);
					break;
				case EXIT:
					enviar_pcb_a_kernel(*socket_cliente, proceso, FINISHED);
					break;
			}
			break;
		case -1:
			log_warning(logger, "El cliente se desconecto. Terminando conexion");
			return;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
}

void liberar_servidor(int *socket_servidor)
{
	log_warning(logger, "Liberando servidor");
	close(*socket_servidor);
	free(socket_servidor);
}
