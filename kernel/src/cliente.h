#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include "logconfig.h"
#include "proceso.h"
#include "server.h"

extern int conexion_cpu;

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

void* serializar_paquete(t_paquete*, int);
int crear_conexion(char*, char*);
void send_handshake(int);
void atender_servidor(int*);
void esperar_servidor_hilos(int);
void enviar_mensaje(char*, int);
void crear_buffer(t_paquete*);
t_paquete* crear_paquete(op_code);
void agregar_a_paquete(t_paquete*, void*, int);
void enviar_paquete(t_paquete*, int);
void eliminar_paquete(t_paquete*);
void enviar_pcb_a_cpu(int, pcb*);
pcb* recibir_pcb_de_cpu(t_list*);
void liberar_conexion(int);

#endif /* CLIENTE_H_ */
