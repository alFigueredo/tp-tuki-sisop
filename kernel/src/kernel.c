#include "kernel.h"
typedef struct {
		int pid;
		t_list* instrucciones;
		int program_counter;
		struct registros_cpu;
		t_list* tabla_segmentos;
		int estimado_proxRafaga;
		int tiempo_llegada_ready;
		t_list* archivos_abiertos;
	}pcb;

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

void esperar_cliente_hilos(int server){
	while (1) {
		   pthread_t thread;
		   int *socket_cliente = malloc(sizeof(int));
		   *socket_cliente = accept(server, NULL, NULL);
		   log_info(logger, "Se conecto un cliente!");
		   recv_handshake(socket_cliente);
		   pthread_create(&thread,NULL,(void*) atender_cliente,socket_cliente);
		   pthread_detach(thread);
		}

}

int atender_cliente(int cliente_fd){
	t_list lista;
	while (1) {
			int cod_op = recibir_operacion(cliente_fd);
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
				log_error(logger, "el cliente se desconecto. Terminando coneccion");
				return EXIT_FAILURE;
			default:
				log_warning(logger,"Operacion desconocida. No quieras meter la pata");
				break;
			}
		}

}
