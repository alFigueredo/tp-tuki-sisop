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
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include "operaciones.h"
#include "cliente.h"
#include "server.h"

extern t_queue* qnew;
extern t_queue* qready;
extern t_queue* qexec;
extern t_queue* qblock;
extern t_queue* qexit;
extern sem_t* fifo_largo_plazo;


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
		int tiempo_llegada_ready;
		t_list* archivos_abiertos;
} pcb;

// t_dictionary* diccionario_registros(registros_cpu*);
void iniciar_colas(void);
void destruir_colas(void);
pcb* generar_proceso(t_list*);
void enviar_pcb(int, pcb*, op_code);
pcb* recibir_pcb(t_list*);
char* queue_iterator(t_queue*);

#endif /* PROCESO_H_ */
