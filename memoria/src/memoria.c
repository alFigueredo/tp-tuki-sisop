#include "memoria.h"

char* ip_memoria ;

int main(int argc, char** argv) {

	if (argc < 2) {
		return EXIT_FAILURE;
	}
	logger = iniciar_logger("./memoria.log", "Memoria");

	// CARGAR CONFIG
	t_config* config                = iniciar_config          (argv[1]);
	config_mem.ip_memoria           = config_get_string_value (config, "IP_MEMORIA");
	config_mem.puerto_escucha       = config_get_string_value (config, "PUERTO_ESCUCHA");
	config_mem.tam_memoria          = config_get_int_value    (config, "TAM_MEMORIA");
	config_mem.tam_segmento_0       = config_get_int_value    (config, "TAM_SEGMENTO_0");
	config_mem.cant_segmentos       = config_get_int_value    (config, "CANT_SEGMENTOS");
	config_mem.retardo_memoria      = config_get_int_value    (config, "RETARDO_MEMORIA");
	config_mem.retardo_compactacion = config_get_int_value    (config, "RETARDO_COMPACTACION");
	config_mem.algoritmo_asignacion = config_get_string_value (config, "ALGORITMO_ASIGNACION");


	iniciar_memoria ();

	//int socket_servidor = -1;
	int socket_servidor = iniciar_servidor(config_mem.puerto_escucha);

	esperar_cliente(socket_servidor);

	terminar_programa(logger, config);
	return EXIT_SUCCESS;
}

void iniciar_memoria () {
	memoria_usuario = malloc (config_mem.tam_memoria);
	lista_tabla_segmentos = list_create();

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
