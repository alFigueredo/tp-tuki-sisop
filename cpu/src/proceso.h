#ifndef PROCESO_H_
#define PROCESO_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <math.h>
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
#include <shared.h>

extern int conexion_kernel;
extern int conexion_memoria;
extern pcb* proceso;
extern t_dictionary* instrucciones;
extern t_dictionary* registros;

typedef enum
{
	I_SET,
	I_MOV_IN,
	I_MOV_OUT,
	I_IO,
	I_F_OPEN,
	I_F_CLOSE,
	I_F_SEEK,
	I_F_READ,
	I_F_WRITE,
	I_F_TRUNCATE,
	I_WAIT,
	I_SIGNAL,
	I_CREATE_SEGMENT,
	I_DELETE_SEGMENT,
	I_YIELD,
	I_EXIT
}enum_instrucciones;

void iniciar_diccionario_instrucciones(void);
void iniciar_diccionario_registros(registros_cpu*);
void destruir_diccionarios();
void interpretar_instrucciones(pcb*);
void instruccion_set(char**, pcb*);
void instruccion_mov_in(char**, pcb*);
void mov_in(t_instruction*);
void instruccion_mov_out(char**, pcb*);
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
void error_exit(pcb*);
char* traducir_dir_logica(char*);

#endif /* PROCESO_H_ */
