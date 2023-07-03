#include "shared.h"

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void *serializar_paquete(t_paquete *paquete, int bytes)
{
	void *magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}

void crear_buffer(t_paquete *paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

void *recibir_buffer(int *size, int socket_cliente)
{
	void *buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void enviar_operacion(int socket_cliente, op_code codigo)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = codigo;

	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = malloc(paquete->buffer->size);

	int bytes = paquete->buffer->size + 2 * sizeof(int);

	void *a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_mensaje(char *mensaje, int socket_cliente, op_code codigo)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = codigo;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2 * sizeof(int);

	void *a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char *buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje: %s", buffer);
	free(buffer);
}

t_paquete *crear_paquete(op_code codigo)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete *paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void *a_enviar = serializar_paquete(paquete, bytes);
	
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete *paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

t_list *recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void *buffer;
	t_list *valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while (desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		char *valor = malloc(tamanio);
		memcpy(valor, buffer + desplazamiento, tamanio);
		desplazamiento += tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

void delay(int milliseconds)
{
	t_temporal *clock = temporal_create();
	while (temporal_gettime(clock) < milliseconds)
		;
	temporal_destroy(clock);
}

void enviar_pcb(int conexion, pcb *proceso, op_code codigo)
{
	t_paquete *paquete = crear_paquete(codigo);

	agregar_a_paquete(paquete, &(proceso->pid), sizeof(unsigned int));

	int cantidad_instrucciones = list_size(proceso->instrucciones);
	agregar_a_paquete(paquete, &cantidad_instrucciones, sizeof(int));

	for (int i = 0; i < cantidad_instrucciones; i++)
	{
		char *instruccion = list_get(proceso->instrucciones, i);
		agregar_a_paquete(paquete, instruccion, strlen(instruccion) + 1);
	}

	agregar_a_paquete(paquete, &(proceso->program_counter), sizeof(int));

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
	
	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
}

void recibir_pcb(t_list *lista, pcb *proceso)
{
	int i = 0;
	memcpy(&(proceso->pid), list_get(lista, i++), sizeof(unsigned int));
	int cantidad_instrucciones;
	memcpy(&(cantidad_instrucciones), list_get(lista, i++), sizeof(int));
	if (proceso->instrucciones) {
		list_destroy_and_destroy_elements(proceso->instrucciones, free);
	}
	proceso->instrucciones = list_slice_and_remove(lista, i, cantidad_instrucciones);
	memcpy(&(proceso->program_counter), list_get(lista, i++), sizeof(int));
	memcpy(proceso->registros.AX, list_get(lista, i++), 4);
	memcpy(proceso->registros.BX, list_get(lista, i++), 4);
	memcpy(proceso->registros.CX, list_get(lista, i++), 4);
	memcpy(proceso->registros.DX, list_get(lista, i++), 4);
	memcpy(proceso->registros.EAX, list_get(lista, i++), 8);
	memcpy(proceso->registros.EBX, list_get(lista, i++), 8);
	memcpy(proceso->registros.ECX, list_get(lista, i++), 8);
	memcpy(proceso->registros.EDX, list_get(lista, i++), 8);
	memcpy(proceso->registros.RAX, list_get(lista, i++), 16);
	memcpy(proceso->registros.RBX, list_get(lista, i++), 16);
	memcpy(proceso->registros.RCX, list_get(lista, i++), 16);
	memcpy(proceso->registros.RDX, list_get(lista, i++), 16);
}

void enviar_instruccion(int conexion, t_instruction* proceso, op_code codigo) {
	t_paquete *paquete = crear_paquete(codigo);

	agregar_a_paquete(paquete, &(proceso->pid), sizeof(unsigned int));
	agregar_a_paquete(paquete, proceso->instruccion, strlen(proceso->instruccion)+1);
	// agregar_a_paquete(paquete, &(proceso->tamanio_dato), sizeof(int));
	// agregar_a_paquete(paquete, proceso->dato, proceso->tamanio_dato);

	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
}

void recibir_instruccion(t_list* lista, t_instruction* proceso) {

	int i=0;

	memcpy(&(proceso->pid), list_get(lista, i++), sizeof(unsigned int));
	proceso->instruccion = (char*)list_remove(lista, i);
	// memcpy(&(proceso->tamanio_dato), list_get(lista, i++), sizeof(int));
	// proceso->dato = list_remove(lista, i);

}

void generar_instruccion(pcb* proceso, t_instruction* instruccion_proceso, char* instruccion) {
	instruccion_proceso->pid = proceso->pid;
	instruccion_proceso->instruccion = instruccion;
	instruccion_proceso->tabla_segmentos = proceso->tabla_segmentos;
}

void replace_r_with_0(char *line)
{
	char *pos = strchr(line, '\r');
	if (pos)
	{
		*pos = '\0';
	}
}
