#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include "server.h"


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
		int tiempo_llegada_ready;
		t_list* archivos_abiertos;
	}pcb;


void iterator(char* value);

#endif /* KERNEL_H_ */
