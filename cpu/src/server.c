#include"server.h"

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

int esperar_cliente(int socket_servidor)
{
	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "¡Se conecto un cliente!");
	recv_handshake(socket_cliente);

	return socket_cliente;
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

void esperar_cliente_hilos(int socket_servidor){
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

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje: %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

void enviar_pcb_a_kernel(int conexion_kernel, pcb* proceso, op_code estado) {
	t_paquete* paquete = crear_paquete(estado);
	agregar_a_paquete(paquete, &(proceso->pid), sizeof(unsigned int));
	for (int i=0; i<list_size(proceso->instrucciones); i++) {
		char* instruccion = list_get(proceso->instrucciones, i);
		agregar_a_paquete(paquete, instruccion, strlen(instruccion)+1);
	}
	agregar_a_paquete(paquete, &(proceso->program_counter), sizeof(int));
	/*
	t_dictionary* registros = diccionario_registros(&proceso->registros);
	t_list* arrRegistros = dictionary_elements(registros);
	log_debug(logger, "%s %s %s %s", (char*)list_get(arrRegistros, 0), (char*)list_get(arrRegistros, 1), (char*)list_get(arrRegistros, 2), (char*)list_get(arrRegistros, 3));
	for (int i=0; i<12; i++) {
		switch (i/4) {
			case 0:
				agregar_a_paquete(paquete, list_get(arrRegistros, i), 4);
				break;
			case 1:
				agregar_a_paquete(paquete, list_get(arrRegistros, i), 8);
				break;
			case 2:
				agregar_a_paquete(paquete, list_get(arrRegistros, i), 16);
				break;
		}
	}
	dictionary_destroy(registros);
	*/
	agregar_a_paquete(paquete, proceso->registros.AX, 4);
	agregar_a_paquete(paquete, proceso->registros.BX, 4);
	agregar_a_paquete(paquete, proceso->registros.CX, 4);
	agregar_a_paquete(paquete, proceso->registros.DX, 4);
	agregar_a_paquete(paquete, proceso->registros.EAX, 8);
	agregar_a_paquete(paquete, proceso->registros.EBX, 8);
	agregar_a_paquete(paquete, proceso->registros.ECX, 8);
	agregar_a_paquete(paquete, proceso->registros.EDX, 8);
	agregar_a_paquete(paquete, proceso->registros.RAX, 16);
	agregar_a_paquete(paquete, proceso->registros.RBX, 16);
	agregar_a_paquete(paquete, proceso->registros.RCX, 16);
	agregar_a_paquete(paquete, proceso->registros.RDX, 16);
	enviar_paquete(paquete, conexion_kernel);
	eliminar_paquete(paquete);
}

void liberar_servidor(int socket_cliente)
{
	log_warning(logger, "Liberando servidor");
	close(socket_cliente);
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

