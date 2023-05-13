#include "cpu.h"

int main(void) {

	int conexion_memoria = -1;

	logger = iniciar_logger("./cpu.log", "CPU");
	config = iniciar_config("/home/utnso/tp-2023-1c-Grupo-Asado/cpu/cpu.config");

	// char* ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	// char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	char* puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");

	/*
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	enviar_mensaje("Intento de conexión del cpu a la memoria", conexion_memoria);
	*/
	int socket_servidor = -1;
	socket_servidor = iniciar_servidor(puerto_escucha);
	esperar_cliente(socket_servidor);

	liberar_conexion(conexion_memoria);
	terminar_programa(logger, config);
	return EXIT_SUCCESS;
}

void terminar_programa(t_log* logger, t_config* config)
{
	if (logger != NULL) {
		log_destroy(logger);
	}
	if (config != NULL) {
		config_destroy(config);
	}
}
