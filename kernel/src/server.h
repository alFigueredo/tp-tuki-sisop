#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/process.h>
#include <commons/collections/list.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "logconfig.h"

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
		int pid;
		t_list* instrucciones;
		int program_counter;
		registros_cpu registros;
		t_list* tabla_segmentos;
		int estimado_proxRafaga;
		int tiempo_llegada_ready;
		t_list* archivos_abiertos;
} pcb;

int iniciar_servidor(char*);
int esperar_cliente(int);
void recv_handshake(int);
void atender_cliente(int*);
void esperar_cliente_hilos(int);
int recibir_operacion(int);
void recibir_mensaje(int);
void* recibir_buffer(int*, int);
t_list* recibir_paquete(int);
void liberar_servidor(int);
void iterator(char*);

#endif /* SERVER_H_ */
