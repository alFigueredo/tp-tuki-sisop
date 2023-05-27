#include "kernel.h"

int main(int argc, char** argv) {

	if (argc < 2) {
	    return EXIT_FAILURE;
	}
	conexion_cpu = -1;
	conexion_memoria = -1;
	conexion_filesystem = -1;

	logger = iniciar_logger("./kernel.log", "Kernel");
	config = iniciar_config(argv[1]);
	iniciar_colas();
	sem_largo_plazo = malloc(sizeof(sem_t));
	sem_exec = malloc(sizeof(sem_t));
	sem_init(sem_largo_plazo, 0, config_get_int_value(config, "GRADO_MAX_MULTIPROGRAMACION"));
	sem_init(sem_exec, 0, 1);
	conexiones = dictionary_create();

	// char* ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	// char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	char* ip_cpu = config_get_string_value(config, "IP_CPU");
	char* puerto_cpu = config_get_string_value(config, "PUERTO_CPU");
	// char* ip_filesystem = config_get_string_value(config, "IP_FILESYSTEM");
	// char* puerto_filesystem = config_get_string_value(config, "PUERTO_FILESYSTEM");
	char* puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");

	/*
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	enviar_mensaje("Intento de conexión del kernel a la memoria", conexion_memoria);
	*/
	conexion_cpu = crear_conexion(ip_cpu, puerto_cpu);
	enviar_mensaje("Intento de conexión del kernel al cpu", conexion_cpu);
	esperar_servidor(conexion_cpu);
	/*
	conexion_filesystem = crear_conexion(ip_filesystem, puerto_filesystem);
	enviar_mensaje("Intento de conexión del kernel al filesystem", conexion_filesystem);
	*/

	int socket_servidor = -1;
	socket_servidor = iniciar_servidor(puerto_escucha);
	esperar_cliente(socket_servidor);

	destruir_colas();
	sem_destroy(sem_largo_plazo);
	sem_destroy(sem_exec);
	free(sem_largo_plazo);
	free(sem_exec);
	dictionary_destroy(conexiones);
	liberar_conexion(conexion_cpu);
	liberar_conexion(conexion_memoria);
	liberar_conexion(conexion_filesystem);
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
