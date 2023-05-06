#include "logconfig.h"

t_log* logger;
t_config* config;

t_log* iniciar_logger(char* log_file)
{
	t_log* nuevo_logger;
	if ((nuevo_logger = log_create(log_file, "logger", 1, LOG_LEVEL_DEBUG)) == NULL) {
	    printf("¡No se pudo crear el logger!\n");
	    exit(1);
	}
	return nuevo_logger;
}

t_config* iniciar_config(char* config_file)
{
	t_config* nuevo_config;
	if ((nuevo_config = config_create(config_file)) == NULL) {
	    printf("¡No se pudo crear el config!\n");
	    exit(1);
	}
	return nuevo_config;
}
