#include "filesystem.h"

t_config* superBloque;
t_bitarray *bitmap;
char **vectorDePathsPCBs;
int cantidadPaths;

int main(int argc, char** argv)
{

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

	superBloque=iniciar_config(config_get_string_value(config,"PATH_SUPERBLOQUE"));

	if (superBloque == NULL)
	{
	    log_error(logger, "Error al inicializar la configuración del superbloque");
	    exit(EXIT_FAILURE);
	}

	size_t tamanioBitmap = config_get_int_value(superBloque, "BLOCK_COUNT") / 8;

	// Abre el archivo Bitmap para trabajar con mmap desde la memoria
	int fd_bitmap = open(config_get_string_value(config, "PATH_BITMAP"),O_RDWR);

	if (fd_bitmap == -1)
	{
	    log_error(logger, "Error al abrir el archivo del bitmap");
	    exit(EXIT_FAILURE);
	}



	void* intermedio = mmap(NULL, tamanioBitmap, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd_bitmap, 0);
	if (intermedio == MAP_FAILED)
	{
	    log_error(logger, "Error al mapear el archivo del bitmap");
	    exit(EXIT_FAILURE);
	}

	//void* espacioBitmap = malloc(tamanioBitmap);
	//fread(espacioBitmap,tamanioBitmap,1,archivoBitmap);
	bitmap = bitarray_create_with_mode(intermedio, tamanioBitmap, LSB_FIRST);

	if (bitmap == NULL)
		{
			log_error(logger, "Error al crear el bitmap");
			exit(EXIT_FAILURE);
		}

	bitarray_destroy(bitmap);

	if (msync(intermedio, tamanioBitmap, MS_SYNC) == 0)
		{
			log_info(logger, "Se escribió correctamente en el bitmap");
		}
		else
		{
			log_warning(logger, "No se pudo escribir correctamente en el bitmap");
		}
		munmap(intermedio,tamanioBitmap);
		close(fd_bitmap);


	/*
	char **vectorDePathsPCBs = NULL;
	int cantFCBs=0;
	//Guardo todas las rutas de los PCBs en un vector
	log_info(logger, config_get_string_value(config,"PATH_FCB"));
	cantFCBs = contarArchivosEnCarpeta(config_get_string_value(config,"PATH_FCB"),&vectorDePathsPCBs);
	printf("La cantidad de FCBS es %d\n",cantFCBs);
	//////////////////////////


	if (abrirArchivo("Notas1erParcialK9999", vectorDePathsPCBs, cantFCBs))
	{
		log_info(logger, "Existe el archivo buscado");
	}
	else
	{
		log_info(logger, "No existe el archivo buscado");
	}

	if (crearArchivo("ISA.FCB",config_get_string_value(config,"PATH_FCB"),&vectorDePathsPCBs,&cantFCBs))
		{
			log_info(logger, "Se creo correctamente el archivo");
		}
		else
		{
			log_info(logger, "no se creo el archivo AYUDAAAAA");
		}
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
		free(*vector);
		free(vector);
	}
}
char* concatenarCadenas(const char* str1, const char* str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);

    char* resultado = (char*)malloc((len1 + len2 + 1) * sizeof(char));

    if (resultado == NULL) {
        perror("Error de asignación de memoria");
        return NULL;
    }

    strcpy(resultado, str1);
    strcat(resultado, str2);

    return resultado;
}
int contarArchivosEnCarpeta(const char *carpeta, char ***vectoreRutas) {
    DIR *dir;
    struct dirent *ent;
    int contador = 0;
    int contador2 = 0;
    char *mediaRutaAbsoluta = concatenarCadenas(carpeta,"/");
    char *rutaAbsoluta;
    dir = opendir(carpeta);

    if (dir == NULL)
    {
        log_info(logger, "No se pudo abrir la carpeta");
        return -1; // Retorna -1 en caso de error
    }

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG)
        { // Verifica si es un archivo regular
        	log_info(logger, "Se leyo un archivo en el primer while");
        	contador++;
        }
    }
    *vectoreRutas = malloc(contador * sizeof(char*));
    closedir(dir);
    dir = opendir(carpeta);
    while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) { // Verifica si es un archivo regular
            	log_info(logger, "Se leyo un archivo en el segundo while");
            	(*vectoreRutas)[(contador2)]=malloc((strlen(ent->d_name) + 1) * sizeof(char) + (strlen(mediaRutaAbsoluta) + 1) * sizeof(char));
            	rutaAbsoluta = concatenarCadenas(mediaRutaAbsoluta,ent->d_name);
            	strcpy((*vectoreRutas)[(contador2)], rutaAbsoluta);
            	printf("El nombre es %s\n", ent->d_name);
            	contador2++;
            	free(rutaAbsoluta);
            }
        }
    closedir(dir);
    free (mediaRutaAbsoluta);
    return contador;
}
int abrirArchivo(char *nombre, char **vectorDePaths,int cantidadPaths)
{
	log_info(logger,"Abrir Archivo: %s",nombre);
	int i=0;
	t_config* config_inicial;
	while (i<cantidadPaths)
	{
		config_inicial= iniciar_config(vectorDePaths[i]);
		if(strcmp(nombre,config_get_string_value(config_inicial,"NOMBRE_ARCHIVO")) == 0)
		{
			config_destroy(config_inicial);
			return 1;
		}
		i++;
		config_destroy(config_inicial);
	}
	return 0;
}
int crearArchivo(char *nombre,char *carpeta, char ***vectoreRutas, int *cantidadPaths)
{
	log_info(logger, "Crear Archivo: %s",nombre);
	FILE* archivo;
	t_config* configArchivo;
	char **vectorPruebas;
	char *mediaRutaAbsoluta = concatenarCadenas(carpeta,"/");
	char *rutaArchivo = malloc ((strlen(nombre) + 1) * sizeof(char) +(strlen(mediaRutaAbsoluta) + 1) * sizeof(char));
	rutaArchivo = concatenarCadenas(mediaRutaAbsoluta, nombre);

	archivo = fopen(rutaArchivo , "w+");
	fclose(archivo);
	configArchivo = iniciar_config(rutaArchivo);
	config_set_value(configArchivo,"NOMBRE_ARCHIVO", nombre);
	config_set_value(configArchivo,"TAMANIO_ARCHIVO", "0");
	config_save(configArchivo);
	vectorPruebas = realloc(*vectoreRutas,(*cantidadPaths + 1) * sizeof(char*));
	if (vectorPruebas != NULL)
	{
		(*vectoreRutas) = vectorPruebas;

		(*vectoreRutas)[*cantidadPaths] = malloc((strlen(rutaArchivo) + 1) * sizeof(char));
		strcpy((*vectoreRutas)[*cantidadPaths],rutaArchivo);
		printf("El nuevo archivo agregado al path de archivos es %s",(*vectoreRutas)[*cantidadPaths]);
		*cantidadPaths = *cantidadPaths + 1;
		config_destroy(configArchivo);
		free (mediaRutaAbsoluta);
		free(rutaArchivo);
		return 1;
	}
	else
	{
		config_destroy(configArchivo);
		free (mediaRutaAbsoluta);
		free(rutaArchivo);
		return 0;
	}
}

int truncarArchivo(char *nombre,char *carpeta, char ***vectoreRutas, int *cantidadPaths, int tamanioNuevo)
{
	log_info(logger, "Truncar Archivo: %s - Tamaño: %d",nombre,tamanioNuevo);
	int i=0;
	t_config* configArchivoActual;
	t_config* configSuperBloque;
	configSuperBloque = iniciar_config(config_get_string_value(config,"PATH_SUPERBLOQUE"));
	int tamanioBloques;
	int tamanioOriginal;
	int cantidadBloquesOriginal;
	int cantidadBloquesNueva;

	while (i<*cantidadPaths)
	{
		configArchivoActual = iniciar_config((*vectoreRutas)[i]);
		if(strcmp(nombre,config_get_string_value(configArchivoActual,"NOMBRE_ARCHIVO")) == 0)
		{
			break;
		}
		i++;
		config_destroy(configArchivoActual);
	}

	tamanioOriginal = config_get_int_value(configArchivoActual,"TAMANIO_ARCHIVO");
	tamanioBloques = config_get_int_value(configSuperBloque,"BLOCK_SIZE");
	config_set_value(configArchivoActual,"TAMANIO_ARCHIVO",string_itoa(tamanioNuevo));
	cantidadBloquesOriginal = dividirRedondeando(tamanioOriginal, tamanioBloques);
	cantidadBloquesNueva = dividirRedondeando(tamanioNuevo, tamanioBloques);

	if (tamanioOriginal < tamanioNuevo)
	{
		log_info(logger,"Se agranda el archivo");
		agregarBloques(cantidadBloquesOriginal,cantidadBloquesNueva,configArchivoActual);
	}
	else
	{
		log_info(logger,"Se achica el archivo");
		sacarBloques(cantidadBloquesOriginal,cantidadBloquesNueva,configArchivoActual, tamanioOriginal);
	}
	return 1;
}

void sacarBloques(int cantidadBloquesOriginal ,int cantidadBloquesNueva,t_config* configArchivoActual,int tamanioOriginal)
{
	int cantidadBloquesAEliminar = cantidadBloquesNueva - cantidadBloquesOriginal;
	if (cantidadBloquesOriginal <= 1)
		{
			//busco un bloque libre para agregar como bloque de punteros
			log_info(logger,"El archivo solo tiene un bloque. No se modifica nada referido a los bloques");
			return;
		}
	uint32_t punteroIndirecto = config_get_int_value(configArchivoActual,"PUNTERO_INDIRECTO");
	uint32_t punteroACadaBloque;
	FILE *bloques = fopen(config_get_string_value(config,"PATH_BLOQUES"),"r+");
	// Me muevo al final del bloque de punteros para eliminar puntero por puntero
	log_info(logger,"Acceso Bloque - Archivo: %s - Bloque Archivo: bloque de punteros - Bloque File System %d",config_get_string_value(config,"NOMBRE_ARCHIVO"),punteroIndirecto);
	fseek(bloques,punteroIndirecto * config_get_int_value(superBloque,"BLOCK_SIZE"),SEEK_SET);
	fseek(bloques,sizeof(uint32_t)*(cantidadBloquesOriginal - 1), SEEK_CUR);

	for(int i=0 ; i < cantidadBloquesAEliminar;i++)
	{


		fseek(bloques,-sizeof(uint32_t),SEEK_CUR);
		fread(&punteroACadaBloque,sizeof(uint32_t),1,bloques);
		log_info(logger,"Acceso a Bitmap - Bloque: %d - Estado: %d",punteroACadaBloque,bitarray_test_bit(bitmap,punteroACadaBloque));
		bitarray_clean_bit(bitmap,punteroACadaBloque);
		log_info(logger,"Bitmap modificado - Bloque: %d Estado nuevo: %d ",punteroACadaBloque,bitarray_test_bit(bitmap,punteroACadaBloque));
		fseek(bloques,-sizeof(uint32_t),SEEK_CUR);


	}
	fclose(bloques);


	return;
}
void agregarBloques(int cantidadBloquesOriginal ,int cantidadBloquesNueva,t_config* configArchivoActual)
{
	int cantidadBloquesAAGregar = cantidadBloquesNueva - cantidadBloquesOriginal;
	if (cantidadBloquesOriginal <= 1 && cantidadBloquesNueva !=1)
		{
			//busco un bloque libre para agregar como bloque de punteros
			log_info(logger,"Se busca bloque libre para agregar los punteros a bloques");
			for(int i=0;i<config_get_int_value(superBloque, "BLOCK_COUNT");i++)
			{
				if (accesoBitmap(bitmap, i))
				{
					//Encontre un bloque vacio lo marco como ocupado
					setearBitmap(bitmap,i);
					config_set_value(configArchivoActual,"PUNTERO_INDIRECTO",i);
					break;

				}
			}
		}
	uint32_t punteroIndirecto = config_get_int_value(configArchivoActual,"PUNTERO_INDIRECTO");
	uint32_t punteroACadaBloque;
	FILE *bloques = fopen(config_get_string_value(config,"PATH_BLOQUES"),"r+");
	log_info(logger,"Acceso Bloque - Archivo: %s - Bloque Archivo: bloque de punteros - Bloque File System %d",config_get_string_value(config,"NOMBRE_ARCHIVO"),punteroIndirecto);

	//Busco los bloques disponibles en el bitmap:




	return;
}
int dividirRedondeando(int numero1 , int numero2)
{
	if(numero1 % numero2 == 0)
	{
		return (numero1)/(numero2);
	}
	else
	{
		return (numero1)/(numero2) + 1;
	}
}
void iniciarArchivoBitmap()
{
	FILE *archivoBitmap=fopen(config_get_string_value(config,"PATH_BITMAP"),"r+");
	void* punteroABitmapPruebas = malloc(config_get_int_value(superBloque, "BLOCK_COUNT") / 8);
	t_bitarray *bitmapPruebas = bitarray_create_with_mode(punteroABitmapPruebas, config_get_int_value(superBloque, "BLOCK_COUNT") / 8, LSB_FIRST);
	fwrite(bitmapPruebas->bitarray,config_get_int_value(superBloque, "BLOCK_COUNT") / 8,1,archivoBitmap);
	fclose(archivoBitmap);
	bitarray_destroy(bitmapPruebas);
	free(punteroABitmapPruebas);
	return;
}
bool accesoBitmap(t_bitarray* bitmapAAcceder, off_t bit_index)
{
	int posicionIndicada = bit_index;
	log_info(logger,"Acceso a Bitmap - Bloque: %d - Estado: %d",posicionIndicada,bitarray_test_bit(bitmapAAcceder,bit_index));
	return bitarray_test_bit(bitmapAAcceder,bit_index);

}
void setearBitmap(t_bitarray* bitmapAAcceder, off_t bit_index)
{
	int posicionIndicada = bit_index;
	log_info(logger,"Acceso a Bitmap - Bloque: %d - Estado: %d",posicionIndicada,bitarray_test_bit(bitmapAAcceder,bit_index));
	bitarray_set_bit(bitmapAAcceder, bit_index);
	return;
}
