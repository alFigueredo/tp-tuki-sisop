#ifndef SHARED_H_
#define SHARED_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/string.h>
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
	EXIT,
	IO_BLOCK,
	WAIT,
	SIGNAL,
	F_OPEN,
	F_CLOSE,
	F_CREATE,
	F_TRUNCATE,
	F_READ,
	F_WRITE,
	F_SEEK,
	CREATE_SEGMENT,
	DELETE_SEGMENT,
	CREATE_SEGMENT,
	DELETE_SEGMENT,
	COMPACTACION,
	MOV_IN,
	MOV_OUT,
	OK,
	EL_ARCHIVO_NO_EXISTE_PAAAAAAA,
	YA_SE_TERMINO_LA_TRUNCACION,
	EXIT_SEG_FAULT,
	EXIT_OUT_OF_MEMORY
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

typedef struct{
	 char AX[4];
	 char BX[4];
	 char CX[4];
	 char DX[4];
	 char EAX[8];
	 char EBX[8];
	 char ECX[8];
	 char EDX[8];
	 char RAX[16];
	 char RBX[16];
	 char RCX[16];
	 char RDX[16];

}registros_cpu;

typedef struct {
	unsigned int pid;
	t_list* instrucciones;
	int program_counter;
	registros_cpu registros;
	t_list* tabla_segmentos;
	int estimado_proxRafaga;
	char* tiempo_llegada_ready;
	t_list* archivos_abiertos;
}pcb;

typedef struct {
	unsigned int pid;
	char* instruccion;
	t_list* tabla_segmentos;
} t_instruction;

int recibir_operacion(int);
void* serializar_paquete(t_paquete*, int);
void crear_buffer(t_paquete*);
void* recibir_buffer(int*, int);
void enviar_operacion(int, op_code);
void enviar_mensaje(char*, int, op_code);
void recibir_mensaje(int);
t_paquete* crear_paquete(op_code);
void agregar_a_paquete(t_paquete*, void*, int);
void enviar_paquete(t_paquete*, int);
void eliminar_paquete(t_paquete*);
t_list* recibir_paquete(int);
void delay(int);
void enviar_pcb(int, pcb*, op_code);
void recibir_pcb(t_list*, pcb*);
void enviar_instruccion(int, t_instruction*, op_code);
void recibir_instruccion(t_list*, t_instruction*);
void generar_instruccion(pcb*, t_instruction*, char*);
void replace_r_with_0(char*);

#endif /* SHARED_H_ */
