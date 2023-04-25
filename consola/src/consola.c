#include "consola.h"

int main(int argc, char** argv)
{
	if (argc < 3) {
        return EXIT_FAILURE;
	}
	int conexion = -1;
	char* ip;
	char* puerto;
	t_log* logger;
	t_config* config;
	logger = iniciar_logger();
	config = iniciar_config(argv[1]);

	ip = config_get_string_value(config, "IP_KERNEL");
	puerto = config_get_string_value(config, "PUERTO_KERNEL");

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Armamos y enviamos el paquete
	paquete_texto(conexion, argv[2]);

	terminar_programa(conexion, logger, config);

}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	if ((nuevo_logger = log_create("consola.log", "logger", true, LOG_LEVEL_INFO)) == NULL) {
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

void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa
	while(1) {
		leido = readline("> ");
	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
		if (leido[0]=='\0') {
			break;
		}
		log_info(logger, "%s", leido);
	// ¡No te olvides de liberar las lineas antes de regresar!
		free(leido);
	}
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete
	while(1) {
		leido = readline("> ");
		if (leido[0]=='\0') {
			break;
		}
		agregar_a_paquete(paquete, leido, strlen(leido)+1);
		free(leido);
	}
	enviar_paquete(paquete, conexion);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	eliminar_paquete(paquete);

}

void paquete_texto(int conexion, char* pseudocodigo)
{
	char leido[64];
	char* parsed;
	t_paquete* paquete = crear_paquete();
	FILE* fptr = fopen(pseudocodigo, "r");
	if (fptr == NULL) {
		printf("¡No se pudo abrir el archivo!\n");
		exit(1);
	}
	while(NULL !=fgets(leido, 64, fptr)) {
		parsed = strtok(leido, "\n");
		agregar_a_paquete(paquete, leido, strlen(parsed)+1);
	}
	enviar_paquete(paquete, conexion);
	// ¡No te olvides de cerrar el archivo y liberar paquete antes de regresar!
	fclose(fptr);
	eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config)
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	if (logger != NULL) {
		log_destroy(logger);
	}
	if (config != NULL) {
		config_destroy(config);
	}
	if (conexion != -1) {
		liberar_conexion(conexion);
	}
}
