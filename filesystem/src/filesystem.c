#include "filesystem.h"

int main(int argc, char** argv) {

	if (argc < 2) {
		return EXIT_FAILURE;
	}
	int conexion_memoria = -1;

	logger = iniciar_logger("./filesystem.log", "FileSystem");
	config = iniciar_config(argv[1]);
//	t_config* superBloque;
//	t_config* fcb1;
//	t_bitarray* bitmap;
//	void* puntero_a_bits=malloc(1);
	char* ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	char* puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");

	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);

	enviar_mensaje("Intento de conexión del filesystem a la memoria", conexion_memoria, MENSAJE);

	//Una vez realizada la coneccion a memoria levanto el bitmap de bloques y recorro FCBs

//	superBloque= config_create(config_get_string_value(config,"PATH_SUPERBLOQUE"));
//	bitmap = bitarray_create_with_mode(puntero_a_bits, 1, LSB_FIRST);


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
