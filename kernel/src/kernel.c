#include "kernel.h"

int main(void) {


	logger = log_create("kernel.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	config = config_create("kernel.config");

	char* puerto = config_get_string_value(config, "PUERTO_ESCUCHA");

	int server_fd = iniciar_servidor(puerto);
	log_info(logger, "Servidor listo para recibir al cliente");

	esperar_cliente_hilos(server_fd);


	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);

}

int atender_cliente(int cliente_fd){
	t_list *lista;
	pcb proceso;
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
				log_warning(logger,"Operacion didesconocida. No quieras meter la pata");
				break;
			}
			(proceso.pid)=rand();
			proceso.instrucciones=lista;
			proceso.program_counter=0;
			proceso.estimado_proxRafaga= config_get_int_value(config,"ESTIMACION_INICIAL");
			log_info(logger, "Se crea el proceso %d en NEW",proceso.pid);


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




