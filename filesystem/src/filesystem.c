#include "filesystem.h"

int main(int argc, char** argv) {

	if (argc < 2)
	{
		return EXIT_FAILURE;
	}
	int conexion_memoria = -1;
	logger = iniciar_logger("./filesystem.log", "FileSystem");
	log_info(logger, "logg iniciado");
	config = iniciar_config(argv[1]);

	/*
	char* ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	int socket_servidor = -1;
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	enviar_mensaje("Intento de conexión del filesystem a la memoria", conexion_memoria, MENSAJE);
	*/

	//Una vez realizada la coneccion a memoria levanto el bitmap de bloques y recorro FCBs
	// Trabajo sobre file System exclyuyendo conexiones
	int cantFCBs=0;/*
	t_config* superBloque=iniciar_config(config_get_string_value(config,"PATH_SUPERBLOQUE"));
	char **vectorDePathsPCBs = NULL;
	t_bitarray* bitmap;
	int tamanio_bitmap = ((config_get_int_value(superBloque,"BLOCK_COUNT"))/8);

	//Abro el archivo Bitmap para luego trabajarlo con mmap desde memoria
	int fd_bitmap = open(config_get_string_value(config,"PATH_BITMAP"),O_CREAT | O_RDWR);
	void* intermedio = mmap(NULL,tamanio_bitmap,PROT_READ | PROT_WRITE,MAP_SHARED,fd_bitmap,0);

	bitmap = bitarray_create_with_mode(intermedio, tamanio_bitmap, LSB_FIRST);
	*/
	char **vectorDePathsPCBs = NULL;
	//Guardo todas las rutas de los PCBs en un vector
	log_info(logger, config_get_string_value(config,"PATH_FCB"));
	cantFCBs = contarArchivosEnCarpeta(config_get_string_value(config,"PATH_FCB"),&vectorDePathsPCBs);
	//recorrerFCBs(config_get_string_value(config,"PATH_FCB"),vectorDePathsPCBs);
	printf("La cantidad de FCBS es %d\n",cantFCBs);
    printf("La primera ruta es %s\n",vectorDePathsPCBs[0]);
	/*
	if (existeArchivo("Notas1erParcialK9999", vectorDePathsPCBs, &cantFCBs))
	{
		log_info(logger, "Existe el archivo buscado");
	}
	else
	{
		log_info(logger, "No existe el archivo buscado");
	}*/
	/*
	char* puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	socket_servidor = iniciar_servidor(puerto_escucha);
	esperar_cliente(socket_servidor);

	liberar_conexion(conexion_memoria);
	*/
	//terminar_programa(logger, config,vectorDePathsPCBs);

	return EXIT_SUCCESS;
}

void terminar_programa(t_log* logger, t_config* config, char **vector)
{
	if (logger != NULL)
	{
		log_destroy(logger);
	}
	if (config != NULL)
	{
		config_destroy(config);
	}
	if (vector != NULL)
	{
		free(vector);
	}
}
int recorrerFCBs(char *pathDirectorioPCBs,char **file_paths)
{
	DIR *dir;
	struct dirent *ent;

	const char *folder_path = pathDirectorioPCBs;

	dir = opendir(folder_path);
	if (dir == NULL)
	{
		perror("No se pudo abrir la carpeta");
	    return -1;
	}
    int num_files = 0;

	while ((ent = readdir(dir)) != NULL)
	{
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
		{
			continue;
		}

		const char *file_name = ent->d_name;

		// Obtener la longitud de la ruta de la carpeta y del nombre del archivo
		int folder_path_length = strlen(folder_path);
		int file_name_length = strlen(file_name);

		// Alocar memoria para la ruta absoluta en el vector de strings
		char *file_path = malloc((folder_path_length + file_name_length + 2) * sizeof(char));

		// Construir la ruta absoluta concatenando la carpeta y el nombre del archivo
		snprintf(file_path, folder_path_length + file_name_length + 2, "%s/%s", folder_path, file_name);

		// Agregar la ruta absoluta al vector de rutas
		file_paths = realloc(file_paths, (num_files + 1) * sizeof(char *));
		file_paths[num_files] = file_path;

		num_files++;
	}

	closedir(dir);
	return num_files;
}
int contarArchivosEnCarpeta(const char *carpeta, char ***vectoreRutas) {
    DIR *dir;
    struct dirent *ent;
    int contador = 0;
    int contador2 = 0;

    dir = opendir(carpeta);
    if (dir == NULL) {
        log_info(logger, "No se pudo abrir la carpeta");
        return -1; // Retorna -1 en caso de error
    }

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) { // Verifica si es un archivo regular
        	contador++;
        }
    }
    *vectoreRutas = malloc(contador * sizeof(char*));
    closedir(dir);
    dir = opendir(carpeta);
    while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) { // Verifica si es un archivo regular
            	contador2++;
            	*vectoreRutas[contador-1]=malloc((strlen(ent->d_name) + 1) * sizeof(char));
            	strcpy(*vectoreRutas[contador-1], ent->d_name);
            	printf("El nombre es %s\n", ent->d_name);
            }
        }
    closedir(dir);
    return contador;
}
