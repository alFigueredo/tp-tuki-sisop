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

	int reuse = 1;
	if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		log_error(logger, "Error al configurar SO_REUSEADDR");
		abort();
	}

	// Asociamos el socket a un puerto

	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes

	if (listen(socket_servidor, SOMAXCONN) == -1) {
		log_error(logger, "¡No se pudo iniciar el servidor!");
		abort();
	}

	freeaddrinfo(servinfo);
	log_info(logger, "Servidor listo para recibir al cliente");

	return socket_servidor;
}

void recv_handshake(int socket_cliente)
{
	uint32_t handshake;
	uint32_t resultOk = 0;
	uint32_t resultError = -1;
	recv(socket_cliente, &handshake, sizeof(uint32_t), MSG_WAITALL);
	if(handshake == 1)
	   send(socket_cliente, &resultOk, sizeof(uint32_t), 0);
	else {
	   send(socket_cliente, &resultError, sizeof(uint32_t), 0);
	}
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
	while (1) {
		int cod_op = recibir_operacion(*socket_cliente);
		switch (cod_op) {
			case MENSAJE:
				recibir_mensaje(*socket_cliente);
				break;
			case PAQUETE:
				lista = recibir_paquete(*socket_cliente);
				log_info(logger, "Me llegaron los siguientes valores:");
				list_iterate(lista, (void*) iterator);
				list_destroy_and_destroy_elements(lista, free);
				break;
			case NEW:
				lista = recibir_paquete(*socket_cliente);
				generar_proceso(lista, socket_cliente);
				enviar_pcb(conexion_memoria,((pcb*)queue_peek(qnew)),NEW);
				// list_destroy_and_destroy_elements(lista, free);
				break;
			case READY:
				lista = recibir_paquete(*socket_cliente);
				buscar_proceso(qready,lista);
				((pcb*)queue_peek(qnew))->tabla_segmentos = list_get(lista,0); //Actualiza la tabla de segmentos
				new_a_ready(); //Memoria dice que el proceso está listo

			case CREATE_SEGMENT:
				lista = recibir_paquete(*socket_cliente); //Debería enviar la base/id_segmento + el tipo de resultado que se obtuvo: 0-> Todo bien, 1->No hay espacio, 2->Requiere compactacion
				evaluar_respuesta(list_get(lista,1) ,list_get(lista,2)); 
			case DELETE_SEGMENT:
				lista = recibir_paquete(socket_cliente);
				((pcb*)queue_peek(qexec))->tabla_segmentos = list_get(lista,0); //Actualiza la tabla de segmentos
				enviar_pcb(conexion_cpu,((pcb*)queue_peek(qexec)),EXEC);
			case EXEC:
				enviar_mensaje("Traeme las tablas de segmentos",conexion_memoria,COMPACTACION);
			case COMPACTACION:
				lista = recibir_paquete(*socket_cliente);
				actualizar_tablas(lista);
				char* instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				enviar_segmento(instruccion,((pcb*)queue_peek(qexec))->tabla_segmentos);
			case -1:
				log_warning(logger, "El cliente se desconecto. Terminando conexion");
				free(socket_cliente);
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
