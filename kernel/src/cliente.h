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
#include "proceso.h"
#include "server.h"

extern int conexion_cpu;
extern int conexion_memoria;
extern int conexion_filesystem;

int crear_conexion(char*, char*);
void send_handshake(int);
void esperar_servidor(int);
void atender_servidor(int*);
void proceso_ready(pcb*, char*);
void proceso_exec();
void proceso_exit(pcb*);
void liberar_conexion(int);

#endif /* CLIENTE_H_ */
