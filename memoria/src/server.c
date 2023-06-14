#include"server.h"
#include "memoria.h"

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
		log_error(logger, "Error al configurar SO_REUSEADDR \n");
		abort();
	}

	// Asociamos el socket a un puerto

	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes

	if (listen(socket_servidor, SOMAXCONN) == -1) {
		log_error(logger, "¡No se pudo iniciar el servidor! \n");
		abort();
	}

	freeaddrinfo(servinfo);
	log_info(logger, "Servidor listo para recibir al cliente \n");

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

	//pasar manejo_instrucciones

	while (1) {
		int cod_op = recibir_operacion(*socket_cliente);

		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(*socket_cliente);
			break;
		case PAQUETE:
			lista = recibir_paquete(*socket_cliente);

			log_info(logger, "Me llegaron las siguientes instrucciones:");
			list_iterate(lista, (void*) iterator);

			//manejo_instrucciones (lista, *socket_cliente);

			list_clean_and_destroy_elements(lista, free);

			break;
	/*	case READ:                BORRAR ???                                                                 //Ante un pedido de lectura, devolver el valor que se encuentra en la posicion pedida.
					usleep (config_mem.retardo_memoria * 1000);                                            //Tiempo de espera
						//Microsegundos = Milisegundos * 1000
					valor = leer_memoria (dir_dada);                                               		   //Busca y retorna el valor en la direccion de memoria dada
					log_info (logger, "Se leyo el valor %d en la posicion %d", valor, dir_dada);

					//AGREGAR TD LO DE PAQUETES !!!

					break;

		case WRITE:                                                                                // Ante un pedido de escritura, escribir lo indicado en la posición pedida y responder un mensaje de ‘OK’.
					nuevo_valor = *(uint32_t*) list_get(instrucciones,2);

					usleep (config_mem.retardo_memoria * 1000);
					escribir_memoria (nuevo_valor, dir_dada);
					//responder con un msj OK !!!
					log_info (logger, "Se escribio el valor %d en la posicion %d", nuevo_valor, dir_dada);
					break;*/

		case -1:
			log_warning(logger, "El cliente se desconecto. Terminando conexion \n");
			free(socket_cliente);
			return;
		default:
			log_warning(logger,"Operacion desconocida \n");
			break;
		}
	}
}

void liberar_servidor(int *socket_servidor)
{
	log_warning(logger, "Liberando servidor \n");
	close(*socket_servidor);
	free(socket_servidor);
}
