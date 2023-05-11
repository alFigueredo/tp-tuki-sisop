#ifndef OPERACIONES_H_
#define OPERACIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "logconfig.h"

typedef enum
{
	MENSAJE,
	PAQUETE,
	NEW,
	READY,
	EXEC,
	BLOCK,
	FINISHED
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

int recibir_operacion(int);
void* serializar_paquete(t_paquete*, int);
void crear_buffer(t_paquete*);
void* recibir_buffer(int*, int);
void enviar_mensaje(char*, int);
void recibir_mensaje(int);
t_paquete* crear_paquete(op_code);
void agregar_a_paquete(t_paquete*, void*, int);
void enviar_paquete(t_paquete*, int);
void eliminar_paquete(t_paquete*);
t_list* recibir_paquete(int);

#endif /* OPERACIONES_H_ */
