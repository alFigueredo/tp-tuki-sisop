#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "logconfig.h"
#include "process.h"

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
