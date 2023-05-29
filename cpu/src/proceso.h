#ifndef PROCESO_H_
#define PROCESO_H_

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
#include <commons/collections/dictionary.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <operaciones.h>

typedef enum
{
	SET,
	MOV_IN,
	MOV_OUT,
	I_O,
	F_OPEN,
	F_CLOSE,
	F_SEEK,
	F_READ,
	F_WRITE,
	F_TRUNCATE,
	IWAIT,
	ISIGNAL,
	CREATE_SEGMENT,
	DELETE_SEGMENT,
	YIELD,
	IEXIT
}enum_instrucciones;

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
	int pid;
	t_list* instrucciones;
	int program_counter;
	registros_cpu registros;
	t_list* tabla_segmentos;
	int estimado_proxRafaga;
	char* tiempo_llegada_ready;
	t_list* archivos_abiertos;
}pcb;

pcb* recibir_pcb(t_list*);
void enviar_pcb(int, pcb*, op_code);
void delay(int);
t_dictionary* diccionario_instrucciones(void);
t_dictionary* diccionario_registros(registros_cpu*);
void destruir_diccionarios(t_dictionary*, t_dictionary*);
enum_instrucciones interpretar_instrucciones(pcb*);
void instruccion_set(t_dictionary*, char**, pcb*);
void instruccion_mov_in(t_dictionary*, char**, pcb*);
void instruccion_mov_out(t_dictionary*, char**, pcb*);
void instruccion_i_o(char**, pcb*);
void instruccion_f_open(char**, pcb*);
void instruccion_f_close(char**, pcb*);
void instruccion_f_seek(char**, pcb*);
void instruccion_f_read(char**, pcb*);
void instruccion_f_write(char**, pcb*);
void instruccion_f_truncate(char**, pcb*);
void instruccion_wait(char**, pcb*);
void instruccion_signal(char**, pcb*);
void instruccion_create_segment(char**, pcb*);
void instruccion_delete_segment(char**, pcb*);
void instruccion_yield(char**, pcb*);
void instruccion_exit(char**, pcb*);


#endif /* PROCESO_H_ */
