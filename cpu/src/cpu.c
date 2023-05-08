#include "cpu.h"

int main(void) {

	int conexion_memoria = -1;

	logger = iniciar_logger("cpu.log");
	config = iniciar_config("cpu.config");

	interpretar_instruccion();

	char* ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	char* puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");

	// Creamos una conexión hacia el servidor
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);

	enviar_mensaje("Intento de conexión del cpu a la memoria", conexion_memoria);

	int socket_servidor = -1;
	socket_servidor = iniciar_servidor(puerto_escucha);
	esperar_cliente_hilos(socket_servidor);

	liberar_conexion(conexion_memoria);
	terminar_programa(logger, config, socket_servidor);
	return EXIT_SUCCESS;
}

void terminar_programa(t_log* logger, t_config* config, int socket_servidor)
{
	if (socket_servidor != -1) {
		liberar_servidor(socket_servidor);
	}
	if (logger != NULL) {
		log_destroy(logger);
	}
	if (config != NULL) {
		config_destroy(config);
	}
}
