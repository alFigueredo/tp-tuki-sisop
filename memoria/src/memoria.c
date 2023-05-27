#include "memoria.h"

int main(int argc, char** argv) {

	if (argc < 2) {
		return EXIT_FAILURE;
	}
	logger = iniciar_logger("./memoria.log", "Memoria");
	config = iniciar_config(argv[1]);
	char* puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	int socket_servidor = -1;
	socket_servidor = iniciar_servidor(puerto_escucha);
	esperar_cliente(socket_servidor);
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
