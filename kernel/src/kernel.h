#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include "server.h"

void iterator(char* value);
int atender_cliente(int cliente_fd){
	t_list *lista;
	while (1) {
			int cod_op = recibir_operacion(cliente_fd);
			log_info(logger,"%d",cod_op);
			switch (cod_op) {
			case MENSAJE:
				recibir_mensaje(cliente_fd);
				break;
			case PAQUETE:
				lista = recibir_paquete(cliente_fd);
				log_info(logger, "Me llegaron los siguientes valores:");
				list_iterate(lista, (void*) iterator);
				break;
			case -1:
				log_error(logger, "el cliente se desconecto. Terminando conexion");
				return EXIT_FAILURE;
			default:
				log_warning(logger,"Operacion desconocida. No quieras meter la pata");
				break;
			}
		}

}
void esperar_cliente_hilos(int server){
	while (1) {
		   pthread_t thread;
		   int *socket_cliente = malloc(sizeof(int));
		   socket_cliente = accept(server, NULL, NULL);
		   log_info(logger, "Se conecto un cliente!");
		   recv_handshake(socket_cliente);
		   pthread_create(&thread,NULL,(void*) atender_cliente,socket_cliente);
		   pthread_detach(thread);
		}

}
#endif /* KERNEL_H_ */
