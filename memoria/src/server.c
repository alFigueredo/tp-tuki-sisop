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
	t_instruction* proceso;
	void *informacionALeerOEscribir;
	char * instruccion;
	char** parsed;
	char** dir_fisica;
	int id_seg;
	int desp;
	//char* valor_mem;
//	char* nuevo_valor;

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

			//kernel
		/*case CREATE_SEGMENT:
			lista = recibir_paquete(*socket_cliente);
			proceso = malloc(sizeof(t_instruction));
			recibir_instruccion(lista,proceso);

			break;
		case DELETE_SEGMENT:
			break;*/
		case F_READ:
			lista = recibir_paquete(*socket_cliente);
			proceso = malloc(sizeof(t_instruction));
			recibir_instruccion(lista,proceso);
			informacionALeerOEscribir = proceso->dato;
			instruccion = proceso->instruccion;
			parsed = string_split(instruccion," ");
			dir_fisica = string_get_string_as_array(parsed[1]);
			id_seg = atoi(dir_fisica[0]);
			desp = atoi(dir_fisica[1]);
			
			escribir_memoria(id_seg,desp,informacionALeerOEscribir,proceso->tamanio_dato);
			//Avisarle a filesystem que se escribio joya
			enviar_instruccion(*socket_cliente,proceso,OK);
			free(proceso);
			list_destroy_and_destroy_elements(lista, free);
			break;
		case F_WRITE:
			lista = recibir_paquete(*socket_cliente);
			proceso = malloc(sizeof(t_instruction));
			recibir_instruccion(lista,proceso);
			informacionALeerOEscribir = proceso->dato;
			instruccion = proceso->instruccion;
			parsed = string_split(instruccion," ");
			dir_fisica = string_get_string_as_array(parsed[1]);
			id_seg = atoi(dir_fisica[0]);
			desp = atoi(dir_fisica[1]);
			informacionALeerOEscribir = leer_memoria(id_seg,desp,proceso->tamanio_dato);
			proceso->dato=informacionALeerOEscribir;
			enviar_instruccion(*socket_cliente,proceso,ACA_TENES_LA_INFO_GIIIIIIL);
			free(proceso);
			list_destroy_and_destroy_elements(lista, free);
			break;
			//cpu
		case MOV_IN: //leer cpu
			lista = recibir_paquete(*socket_cliente);
			proceso = malloc(sizeof(t_instruction));
			recibir_instruccion(lista,proceso);

		//	instruccion = proceso->instruccion;
		//	parsed = string_split(instruccion," ");
		//	dir_fisica = string_get_string_as_array(parsed[2]);
			//----------------------------------------------------
		//	id_seg = atoi(dir_fisica[0]);
	//		desp = atoi(dir_fisica[1]);

			//valor_mem = leer_memoria(id_seg, desp);

			//log_info(logger, "PID: %u - Accion: LEER - Direccion fisica: (%d - %d) - Tamanio: %d - Origen: CPU", proceso->pid, id_seg, desp);
			
			list_destroy_and_destroy_elements(lista, free);

			break;
		case MOV_OUT: //escribir
			//parsed [1] -> dir fisica
			// parsed [2 -> vslor
			lista = recibir_paquete(*socket_cliente);
			proceso = malloc(sizeof(t_instruction));
			recibir_instruccion(lista,proceso);

		//	instruccion = proceso->instruccion;
		//	parsed = string_split(instruccion," ");
	//		dir_fisica = string_get_string_as_array(parsed[1]);
		//	id_seg = atoi(dir_fisica[0]);
			//desp = atoi(dir_fisica[1]);

		//	nuevo_valor = string_get_string_as_array(parsed[2]);	//warning: assignment to ‘char *’ from incompatible pointer type ‘char **’ [-Wincompatible-pointer-types]

			//escribir_memoria(id_seg, desp, nuevo_valor);

			enviar_operacion(*socket_cliente, OK);

		//	log_info(logger, "PID: %u - Accion: ESCRIBIR - Direccion fisica: (%d - %d) - Tamanio: %d - Origen: CPU", proceso->pid, id_seg, desp);

			list_destroy_and_destroy_elements(lista, free);

			break;

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
