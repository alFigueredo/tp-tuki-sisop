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
	I_WAIT,
	I_SIGNAL,
	CREATE_SEGMENT,
	DELETE_SEGMENT,
	YIELD,
	I_EXIT
}enum_instrucciones;

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
