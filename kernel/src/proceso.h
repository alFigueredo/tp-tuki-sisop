#ifndef PROCESO_H_
#define PROCESO_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/process.h>
#include <commons/string.h>
#include <commons/temporal.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <operaciones.h>

extern int conexion_cpu;
extern int conexion_memoria;
extern int conexion_filesystem;
extern t_queue* qnew;
extern t_queue* qready;
extern t_queue* qexec;
extern t_queue* qblock;
extern t_queue* qexit;
extern sem_t* sem_largo_plazo;
extern sem_t* sem_cpu;
extern sem_t* sem_new;
extern sem_t* sem_ready;
extern sem_t* sem_exec;
extern sem_t* sem_block;
extern sem_t* sem_exit;
extern sem_t* sem_new_ready;
extern sem_t* sem_exec_exit;
extern t_temporal* tiempo_en_cpu;
extern t_dictionary* conexiones;

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
} registros_cpu;

typedef struct {
		unsigned int pid;
		t_list* instrucciones;
		int program_counter;
		registros_cpu registros;
		t_list* tabla_segmentos;
		int estimado_proxRafaga;
		char* tiempo_llegada_ready;
		t_list* archivos_abiertos;
} pcb;

void iniciar_colas(void);
void destruir_colas(void);
void iniciar_semaforos(void);
void destruir_semaforos(void);
sem_t* iniciar_semaforo(int, unsigned int);
void destruir_semaforo(sem_t*);
void delay(int);
void queue_extract(t_queue*, pcb*);
void generar_proceso(t_list*, int*);
void enviar_pcb(int, pcb*, op_code);
void recibir_pcb(t_list*);
void new_a_ready(void);
void ready_a_exec(void);
void exec_a_ready(void);
pcb* exec_a_block(void);
void block_a_ready(pcb*);
void exec_a_exit(void);
void planificador(t_queue*);
void calcular_estimacion(pcb*, int64_t);
bool HRRN_comparator(void*, void*);
double HRRN_R(pcb*);
int seconds_from_string_time(char*);
void io_block(void);
char* queue_iterator(t_queue*);

#endif /* PROCESO_H_ */
