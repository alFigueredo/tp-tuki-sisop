#include "cliente.h"

int conexion_cpu;

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

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

void esperar_servidor_hilos(int conexion){
	pthread_t thread;
	int* socket_servidor = malloc(sizeof(int));
	*socket_servidor = conexion;
	pthread_create(&thread,
	               NULL,
	               (void*) atender_servidor,
	               socket_servidor);
	pthread_detach(thread);
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}


void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(op_code codigo)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void enviar_pcb_a_cpu(int conexion_cpu, pcb* proceso) {
	t_paquete* paquete = crear_paquete(EXEC);
	agregar_a_paquete(paquete, &(proceso->pid), sizeof(unsigned int));
	for (int i=0; i<list_size(proceso->instrucciones); i++) {
		char* instruccion = list_get(proceso->instrucciones, i);
		agregar_a_paquete(paquete, instruccion, strlen(instruccion)+1);
	}
	agregar_a_paquete(paquete, &(proceso->program_counter), sizeof(int));
	/*
	t_dictionary* registros = diccionario_registros(&proceso->registros);
	t_list* arrRegistros = dictionary_elements(registros);
	log_trace(logger, "TRACE");
	list_iterate(arrRegistros, (void*) iterator);
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
	enviar_paquete(paquete, conexion_cpu);
	eliminar_paquete(paquete);
}

pcb* recibir_pcb_de_cpu(t_list* lista) {
	pcb* proceso = malloc(sizeof(pcb));
	memcpy(&(proceso->pid), list_remove(lista, 0), sizeof(unsigned int));
	proceso->instrucciones = list_take_and_remove(lista, list_size(lista)-13);
	memcpy(&(proceso->program_counter), list_remove(lista, 0), sizeof(int));
	/*
	t_dictionary* registros = diccionario_registros(&proceso->registros);
	t_list* arrRegistros = dictionary_elements(registros);
	for (int i=0; i<12; i++) {
		switch (i/4) {
			case 0:
				memcpy(list_get(arrRegistros,i), list_remove(lista, 0), 4);
				break;
			case 1:
				memcpy(list_get(arrRegistros,i), list_remove(lista, 0), 8);
				break;
			case 2:
				memcpy(list_get(arrRegistros,i), list_remove(lista, 0), 16);
				break;
		}
	}
	dictionary_destroy(registros);
	*/
	memcpy(proceso->registros.AX, list_remove(lista, 0), 4);
	memcpy(proceso->registros.BX, list_remove(lista, 0), 4);
	memcpy(proceso->registros.CX, list_remove(lista, 0), 4);
	memcpy(proceso->registros.DX, list_remove(lista, 0), 4);
	memcpy(proceso->registros.EAX, list_remove(lista, 0), 8);
	memcpy(proceso->registros.EBX, list_remove(lista, 0), 8);
	memcpy(proceso->registros.ECX, list_remove(lista, 0), 8);
	memcpy(proceso->registros.EDX, list_remove(lista, 0), 8);
	memcpy(proceso->registros.RAX, list_remove(lista, 0), 16);
	memcpy(proceso->registros.RBX, list_remove(lista, 0), 16);
	memcpy(proceso->registros.RCX, list_remove(lista, 0), 16);
	memcpy(proceso->registros.RDX, list_remove(lista, 0), 16);
	return proceso;
}

void liberar_conexion(int socket_cliente)
{
	if (socket_cliente != -1) {
	log_warning(logger, "Liberando conexion");
	close(socket_cliente);
	}
}
