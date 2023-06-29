#include "filesystem.h"

t_config* superBloque;
t_bitarray *bitmap;
char **vectorDePathsPCBs;
int cantidadPaths;
void* memoriaMapeada;

int main(int argc, char** argv)
{

	if (argc < 2)
	{
		return EXIT_FAILURE;
	}
	//int conexion_memoria = -1;
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

	cantidadPaths = contarArchivosEnCarpeta(config_get_string_value(config,"PATH_FCB"),&vectorDePathsPCBs);

	iniciarArchivoBitmap();
	size_t tamanioBitmap = config_get_int_value(superBloque, "BLOCK_COUNT") / 8;

	// Abre el archivo Bitmap para trabajar con mmap desde la memoria
	int fd_bitmap = open(config_get_string_value(config, "PATH_BITMAP"),O_RDWR);

	if (fd_bitmap == -1)
	{
	    log_error(logger, "Error al abrir el archivo del bitmap");
	    exit(EXIT_FAILURE);
	}



	memoriaMapeada = mmap(NULL, tamanioBitmap, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd_bitmap, 0);
	if (memoriaMapeada == MAP_FAILED)
	{
	    log_error(logger, "Error al mapear el archivo del bitmap");
	    exit(EXIT_FAILURE);
	}

	bitmap = bitarray_create_with_mode(memoriaMapeada, tamanioBitmap, LSB_FIRST);

	if (bitmap == NULL)
		{
			log_error(logger, "Error al crear el bitmap");
			exit(EXIT_FAILURE);
		}

	// PRUEBAS CON UN ARCHIVO GENERICO


	if(abrirArchivo("archivoPruebas2",vectorDePathsPCBs,cantidadPaths))
	{
		log_info(logger,"Abrir archivo retorna OK");
	}
	else
	{
		log_info(logger,"Abrir archivo retorna EL_ARCHIVO_NO_EXISTE_PAAAAAAA");
	}
	//////////////////////////////////////
	if(crearArchivo("archivoPruebas2", config_get_string_value(config,"PATH_FCB"), &vectorDePathsPCBs, &cantidadPaths))
	{
		log_info(logger,"Se creo joya el archivo");
	}
	else
	{
		log_error(logger,"No se pudo crear el archivo pibe. Algo se rompio zarpado");
	}
	//////////////////////////////////////
	if(abrirArchivo("archivoPruebas2",vectorDePathsPCBs,cantidadPaths))
	{
		log_info(logger,"Abrir archivo retorna OK");
	}
	else
	{
		log_info(logger,"Abrir archivo retorna EL_ARCHIVO_NO_EXISTE_PAAAAAAA");
	}
	//////////////////////////////////////
	if(truncarArchivo("archivoPruebas2", config_get_string_value(config,"PATH_FCB"), vectorDePathsPCBs, cantidadPaths, 1024))
	{
		log_info(logger,"En teoria el archivo deberia estar truncado");
	}
	else
	{
		log_warning(logger,"El archivo no se pudo truncar");
	}


	// Pruebas genericas PARTE 2

	//Reviso el estado del bitmap
	revisarBitmap(10);
	/////////////////////////

	if(truncarArchivo("archivoPruebas2", config_get_string_value(config,"PATH_FCB"), vectorDePathsPCBs, cantidadPaths, 320))
	{
		log_info(logger,"En teoria el archivo deberia estar truncado");
	}
	else
	{
		log_warning(logger,"El archivo no se pudo truncar");
	}

	// Escribo algo en el archivo para ver que lee

	//Opcion 1
	char* infoPrueba = malloc((strlen("Hola estoy escrito en un archivo") + 1) * sizeof(char));
	strcpy(infoPrueba,"Hola estoy escrito en un archivo");

	//Opcion 2
	//infoPrueba = string_from_format("Hola estoy escrito en un archivo");
	
	
	if(escribirArchivo("archivoPruebas2",200,(strlen("Hola estoy escrito en un archivo") + 1) * sizeof(char),120,infoPrueba))
	{
		log_info(logger,"En teoria se deberia haber escrito el archivo");
	}
	else
	{
		log_warning(logger,"El archivo no se pudo escribir");
	}
	
	char *AlgoALeer = leerArchivo("archivoPruebas2",200,(strlen("Hola estoy escrito en un archivo") + 1) * sizeof(char),0);

	log_info(logger,"Lo leido del archivo es %s", AlgoALeer);
	
	




	bitarray_destroy(bitmap);
	munmap(memoriaMapeada,tamanioBitmap);
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
		}if(truncarArchivo("archivoPruebas2", config_get_string_value(config,"PATH_FCB"), vectorDePathsPCBs, cantidadPaths, 128))
	{
		log_info(logger,"En teoria el archivo deberia estar truncado");
	}
	else
	{
		log_warning(logger,"El archivo no se pudo truncar");
	}

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

int truncarArchivo(char *nombre,char *carpeta, char **vectoreRutas, int cantidadPaths, int tamanioNuevo)
{
	log_info(logger, "Truncar Archivo: %s - Tamaño: %d",nombre,tamanioNuevo);
	int i=0;
	t_config* configArchivoActual;
	int tamanioBloques;
	int tamanioOriginal;
	int cantidadBloquesOriginal;
	int cantidadBloquesNueva;

	while (i<cantidadPaths)
	{
		configArchivoActual = iniciar_config(vectoreRutas[i]);
		if(strcmp(nombre,config_get_string_value(configArchivoActual,"NOMBRE_ARCHIVO")) == 0)
		{
			log_info(logger,"Truncar archivo dice: ENCONTRE EL ARCHIVO A TRUNCAR");
			break;
		}
		i++;
		config_destroy(configArchivoActual);
	}

	tamanioOriginal = config_get_int_value(configArchivoActual,"TAMANIO_ARCHIVO");
	tamanioBloques = config_get_int_value(superBloque,"BLOCK_SIZE");;
	config_set_value(configArchivoActual,"TAMANIO_ARCHIVO",string_itoa(tamanioNuevo));
	cantidadBloquesOriginal = dividirRedondeando(tamanioOriginal, tamanioBloques);
	cantidadBloquesNueva = dividirRedondeando(tamanioNuevo, tamanioBloques);
	config_save(configArchivoActual);
	if (tamanioOriginal < tamanioNuevo && cantidadBloquesOriginal != cantidadBloquesNueva)
	{
		log_info(logger,"Se agranda el archivo");
		agregarBloques(cantidadBloquesOriginal,cantidadBloquesNueva,configArchivoActual);
		config_destroy(configArchivoActual);
		return 1;
	}
	else
	{
		if(cantidadBloquesOriginal != cantidadBloquesNueva)
		{
			log_info(logger,"Se achica el archivo");
			sacarBloques(cantidadBloquesOriginal,cantidadBloquesNueva,configArchivoActual, tamanioOriginal);
			config_destroy(configArchivoActual);
			return 1;
		}
	}
	log_info(logger, "No se modificaron la canidad de bloques del archivo");
	config_destroy(configArchivoActual);
	return 1;
}

void sacarBloques(int cantidadBloquesOriginal ,int cantidadBloquesNueva,t_config* configArchivoActual,int tamanioOriginal)
{
	int cantidadBloquesAEliminar =cantidadBloquesOriginal - cantidadBloquesNueva;
	uint32_t punteroIndirecto = config_get_int_value(configArchivoActual,"PUNTERO_INDIRECTO");
	uint32_t punteroACadaBloque;
	size_t tamanioBloque = config_get_int_value(superBloque,"BLOCK_SIZE");
	FILE *bloques = fopen(config_get_string_value(config,"PATH_BLOQUES"),"r+");

	if (cantidadBloquesOriginal <= 1)
	{
		//busco un bloque libre para agregar como bloque de punteros
		log_info(logger,"El archivo solo tiene un bloque. No se modifica nada referido a los bloques");
		return;
	}
	if (cantidadBloquesNueva == 1)
	{
		// Me muevo al ultimo puntero del bloque de punteros para eliminar puntero por puntero
		log_info(logger,"Acceso Bloque - Archivo: %s - Bloque Archivo: bloque de punteros - Bloque File System %d",config_get_string_value(configArchivoActual,"NOMBRE_ARCHIVO"),punteroIndirecto);
		delay(config_get_int_value(config,"RETARDO_ACCESO_BLOQUE"));
		fseek(bloques,punteroIndirecto * tamanioBloque,SEEK_SET);
		fseek(bloques,sizeof(uint32_t)*(cantidadBloquesOriginal - 1), SEEK_CUR);

		for(int i=0;i<cantidadBloquesAEliminar ;i++)
		{
			fseek(bloques,-sizeof(uint32_t), SEEK_CUR);
			fread(&punteroACadaBloque,sizeof(uint32_t),1,bloques);
			limpiarBitmap(bitmap, punteroACadaBloque);
			fseek(bloques,-sizeof(uint32_t), SEEK_CUR);

		}
		limpiarBitmap(bitmap, config_get_int_value(configArchivoActual,"PUNTERO_INDIRECTO"));
		config_remove_key(configArchivoActual,"PUNTERO_INDIRECTO");
		config_save(configArchivoActual);
	}
	if (cantidadBloquesNueva > 1)
	{
		// Me muevo al ultimo puntero del bloque de punteros para eliminar puntero por puntero
		log_info(logger,"Acceso Bloque - Archivo: %s - Bloque Archivo: bloque de punteros - Bloque File System %d",config_get_string_value(configArchivoActual,"NOMBRE_ARCHIVO"),punteroIndirecto);
		delay(config_get_int_value(config,"RETARDO_ACCESO_BLOQUE"));
		fseek(bloques,punteroIndirecto * tamanioBloque,SEEK_SET);
		fseek(bloques,sizeof(uint32_t)*(cantidadBloquesOriginal - 1), SEEK_CUR);

		for(int i=0;i<cantidadBloquesAEliminar ;i++)
		{
			fseek(bloques,-sizeof(uint32_t), SEEK_CUR);
			fread(&punteroACadaBloque,sizeof(uint32_t),1,bloques);
			limpiarBitmap(bitmap, punteroACadaBloque);
			fseek(bloques,-sizeof(uint32_t), SEEK_CUR);

		}
	}
	fclose(bloques);
	return;
}
void agregarBloques(int cantidadBloquesOriginal ,int cantidadBloquesNueva,t_config* configArchivoActual)
{
	int cantidadBloquesAAGregar = cantidadBloquesNueva - cantidadBloquesOriginal;
	FILE *bloques = fopen(config_get_string_value(config,"PATH_BLOQUES"),"r+");
	int punteroABloquePunteros=0;
	uint32_t punteroACadaBloque;
	if(cantidadBloquesOriginal == 0 && cantidadBloquesNueva == 1)
	{
		//Busco un bloque libre para agregar el primer bloque de datos
		log_info(logger,"Se busca bloque libre para agregar como primer bloque");
		for(int i=0;i<config_get_int_value(superBloque, "BLOCK_COUNT");i++)
		{
			if (accesoBitmap(bitmap, i) == 0)
			{
				//Encontre un bloque vacio lo marco como ocupado
				setearBitmap(bitmap,i);
				config_set_value(configArchivoActual,"PUNTERO_DIRECTO",string_itoa(i));
				config_save(configArchivoActual);
				sincronizarBitmap();
				break;

			}
		}
	}
	if (cantidadBloquesOriginal == 0 && cantidadBloquesNueva >1)
		{
			//Busco un bloque libre para agregar el primer bloque de datos
			log_info(logger,"Se busca bloque libre para agregar como primer bloque");
			for(int i=0;i<config_get_int_value(superBloque, "BLOCK_COUNT");i++)
			{
				if (accesoBitmap(bitmap, i) == 0)
				{
					//Encontre un bloque vacio lo marco como ocupado
					setearBitmap(bitmap,i);
					config_set_value(configArchivoActual,"PUNTERO_DIRECTO",string_itoa(i));
					config_save(configArchivoActual);
					sincronizarBitmap();
					break;

				}
			}
			//busco un bloque libre para agregar como bloque de punteros
			log_info(logger,"Se busca bloque libre para agregar los punteros a bloques");
			for(int i=0;i<config_get_int_value(superBloque, "BLOCK_COUNT");i++)
			{
				if (accesoBitmap(bitmap, i) == 0)
				{
					//Encontre un bloque vacio lo marco como ocupado
					setearBitmap(bitmap,i);
					config_set_value(configArchivoActual,"PUNTERO_INDIRECTO",string_itoa(i));
					config_save(configArchivoActual);
					punteroABloquePunteros = i;
					sincronizarBitmap();
					break;

				}
			}
			// Me ubico en el bloque de punteros
			log_info(logger,"Acceso Bloque - Archivo: %s - Bloque Archivo: bloque de punteros - Bloque File System %d",config_get_string_value(configArchivoActual,"NOMBRE_ARCHIVO"),punteroABloquePunteros);
			fseek(bloques,punteroABloquePunteros * config_get_int_value(superBloque,"BLOCK_SIZE"),SEEK_SET);
			delay(config_get_int_value(config,"RETARDO_ACCESO_BLOQUE"));
			log_info(logger,"Busco bloques libres para agregar al archivo");
			//Busco los espacios libres en el bitmap para agregar los bloques
			for (int i=0;i< cantidadBloquesAAGregar -1;i++)
			{
				int j=1;
				int posicion=0;
				while(j != 0)
				{
					j=accesoBitmap(bitmap, posicion);
					posicion++;
				}
				punteroACadaBloque = posicion - 1;
				setearBitmap(bitmap, posicion -1);
				fwrite(&punteroACadaBloque,sizeof(uint32_t),1,bloques);
			}
			fclose(bloques);
			return;
		}
	if(cantidadBloquesOriginal == 1 && cantidadBloquesNueva >1)
	{
		//busco un bloque libre para agregar como bloque de punteros
		log_info(logger,"Se busca bloque libre para agregar los punteros a bloques");
		for(int i=0;i<config_get_int_value(superBloque, "BLOCK_COUNT");i++)
		{
			if (accesoBitmap(bitmap, i) == 0)
			{
				//Encontre un bloque vacio lo marco como ocupado
				setearBitmap(bitmap,i);
				config_set_value(configArchivoActual,"PUNTERO_INDIRECTO",string_itoa(i));
				config_save(configArchivoActual);
				punteroABloquePunteros = i;
				sincronizarBitmap();
				break;

			}
		}
		// Me ubico en el bloque de punteros
		log_info(logger,"Acceso Bloque - Archivo: %s - Bloque Archivo: bloque de punteros - Bloque File System %d",config_get_string_value(configArchivoActual,"NOMBRE_ARCHIVO"),punteroABloquePunteros);
		fseek(bloques,punteroABloquePunteros * config_get_int_value(superBloque,"BLOCK_SIZE"),SEEK_SET);
		delay(config_get_int_value(config,"RETARDO_ACCESO_BLOQUE"));
		for (int i=0;i< cantidadBloquesAAGregar;i++)
		{
			int j=1;
			int posicion=0;
			while(j != 0)
			{
				j=accesoBitmap(bitmap, posicion);
				posicion++;
			}
			setearBitmap(bitmap, posicion -1);
			punteroACadaBloque = posicion - 1;
			fwrite(&punteroACadaBloque,sizeof(uint32_t),1,bloques);
		}
	}
	if(cantidadBloquesOriginal > 1)
	{
		log_info(logger,"Acceso Bloque - Archivo: %s - Bloque Archivo: bloque de punteros - Bloque File System %d",config_get_string_value(configArchivoActual,"NOMBRE_ARCHIVO"),config_get_int_value(configArchivoActual,"PUNTERO_INDIRECTO"));
		fseek(bloques,config_get_int_value(configArchivoActual,"PUNTERO_INDIRECTO") * config_get_int_value(superBloque,"BLOCK_SIZE"),SEEK_SET);
		fseek(bloques,sizeof(uint32_t)*(cantidadBloquesOriginal - 1),SEEK_CUR);
		delay(config_get_int_value(config,"RETARDO_ACCESO_BLOQUE"));
		for (int i=0;i< cantidadBloquesAAGregar;i++)
		{
			int j=1;
			int posicion=0;
			while(j != 0)
			{
				j=accesoBitmap(bitmap, posicion);
						posicion++;
			}
			punteroACadaBloque = posicion - 1;
			fwrite(&punteroACadaBloque,sizeof(uint32_t),1,bloques);
		}
	}
	fclose(bloques);
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
	for(int i=0;i<config_get_int_value(superBloque, "BLOCK_COUNT") / 8;i++)
	{
		bitarray_clean_bit(bitmapPruebas, i);
	}
	// Muevo el puntero al final del archivo
	fseek(archivoBitmap, 0, SEEK_END);
	// Obtengo la posición actual del puntero
	 long int size = ftell(archivoBitmap);
	if(size == 0)
	{
		fwrite(bitmapPruebas->bitarray,config_get_int_value(superBloque, "BLOCK_COUNT") / 8,1,archivoBitmap);
	}
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
	bitarray_set_bit(bitmapAAcceder, bit_index);
	log_info(logger,"Modificacion Bitmap - Bloque: %d - Estado nuevo: %d",posicionIndicada,bitarray_test_bit(bitmapAAcceder,bit_index));
	return;
}
void limpiarBitmap(t_bitarray* bitmapAAcceder, off_t bit_index)
{
	int posicionIndicada = bit_index;
	bitarray_clean_bit(bitmapAAcceder, bit_index);
	log_info(logger,"Modificacion Bitmap - Bloque: %d - Estado nuevo: %d",posicionIndicada,bitarray_test_bit(bitmapAAcceder,bit_index));
}
void sincronizarBitmap()
{
	if (msync(memoriaMapeada, config_get_int_value(superBloque, "BLOCK_COUNT") / 8, MS_SYNC) == 0)
	{
		log_info(logger, "Se escribió correctamente en el bitmap");
	}
	else
	{
		log_warning(logger, "No se pudo escribir correctamente en el bitmap");
	}
}
void revisarBitmap(int hastaDonde)
{
	for(int i=0;i<hastaDonde; i++ )
	{
		accesoBitmap(bitmap, i);
	}
}

int escribirArchivo(char *nombreArchivo,size_t punteroSeek,size_t bytesAEscribir,int direccion,void *memoriaAEscribir)
{
	log_info(logger,"Escribir Archivo: %s - Puntero: %ld - Memoria: %d - Tamaño: %ld",nombreArchivo,punteroSeek,direccion,bytesAEscribir);
	int i=0;
	t_config* configArchivoActual;
	size_t bloqueAEscribir;
	FILE *bloques = fopen(config_get_string_value(config,"PATH_BLOQUES"),"r+");
	size_t tamanioBloque = config_get_int_value(superBloque,"BLOCK_SIZE");

	while (i<cantidadPaths)
	{
		configArchivoActual = iniciar_config(vectorDePathsPCBs[i]);
		if(strcmp(nombreArchivo,config_get_string_value(configArchivoActual,"NOMBRE_ARCHIVO")) == 0)
		{
			log_info(logger,"Escribir  dice: ENCONTRE EL ARCHIVO A escribir");
			break;
		}
		i++;
		config_destroy(configArchivoActual);
	}
	bloqueAEscribir = punteroSeek /tamanioBloque;
	log_info(logger,"El bloque del archivo a escribir es el bloque %ld",bloqueAEscribir);
	if(bloqueAEscribir == 0)
	{
		moverPunteroAbloqueDelArchivo(bloques,configArchivoActual,bloqueAEscribir);
		//Me fijo si todo lo que voy a leer esta en un solo bloque
		if(bytesAEscribir < tamanioBloque || (bytesAEscribir) + (punteroSeek-bloqueAEscribir *tamanioBloque) < tamanioBloque )
		{
			log_info(logger,"La informacion a escribir entra en un solo bloque");
			fseek(bloques,punteroSeek,SEEK_CUR);
			log_info(logger,"Escribiendo en el archivo solicitado");
			fwrite(memoriaAEscribir,bytesAEscribir,1,bloques);
			fclose(bloques);
			return 1;
		}
		//escribo todo lo que puedo en el primer archivo y despues exribo en el segundo en el segundo
		else
		{
			log_info(logger,"La informacion a escribir NO entra en un solo bloque");
			fseek(bloques,punteroSeek,SEEK_CUR);
			fwrite(memoriaAEscribir,tamanioBloque-punteroSeek,1,bloques);
			
			moverPunteroAbloqueDelArchivo(bloques,configArchivoActual,bloqueAEscribir + 1);
			fwrite(memoriaAEscribir +(tamanioBloque-punteroSeek) ,bytesAEscribir - (tamanioBloque-punteroSeek),1,bloques);
			return 1;
		}
	}
	//no tengo que escribir el bloque del puntero directo. Paso a buscar el primer bloque
	else
	{
		moverPunteroAbloqueDelArchivo(bloques,configArchivoActual,bloqueAEscribir);
		//Hay mas de un bloque para leer
		log_info(logger,"Evaluo si la informacion a escribir tiene que ir en mas de un bloque");
		if(bytesAEscribir > tamanioBloque || (bytesAEscribir) + (punteroSeek-bloqueAEscribir *tamanioBloque) > tamanioBloque)
		{
			log_info(logger,"La informacion a escribir NO entra en un solo bloque");
			//Escribo todo lo que puedo del primer bloque
			fwrite(memoriaAEscribir,tamanioBloque-(punteroSeek-bloqueAEscribir * tamanioBloque),1,bloques);


			moverPunteroAbloqueDelArchivo(bloques,configArchivoActual,bloqueAEscribir + 1);
			//Ahora escribo lo que me falta lo que me falta leer del archivo
			fwrite(memoriaAEscribir +(tamanioBloque-punteroSeek),bytesAEscribir - tamanioBloque-(punteroSeek-bloqueAEscribir * tamanioBloque),1,bloques);
			return 1;
		}
		//Hay solo un bloque que leer
		else
		{
			log_info(logger,"La informacion a escribir entra en un solo bloque");
			fwrite(memoriaAEscribir,bytesAEscribir,1,bloques);
			fclose(bloques);
			return 1;
		}
	}
	fclose(bloques);
	return 1;

}

void *leerArchivo(char *nombreArchivo,size_t punteroSeek,size_t bytesALeer, int direccion)
{
	log_info(logger,"Leer Archivo: %s - Puntero: %ld - Memoria: %d - Tamaño: %ld",nombreArchivo,punteroSeek,direccion,bytesALeer);
	int i=0;
	t_config* configArchivoActual;
	size_t bloqueAleer;
	void *infoDelArchivo;
	void *infoDelArchivo2;
	FILE *bloques = fopen(config_get_string_value(config,"PATH_BLOQUES"),"r+");
	void *punteroFinal;
	size_t tamanioBloque = config_get_int_value(superBloque,"BLOCK_SIZE");
	while (i<cantidadPaths)
	{
		configArchivoActual = iniciar_config(vectorDePathsPCBs[i]);
		if(strcmp(nombreArchivo,config_get_string_value(configArchivoActual,"NOMBRE_ARCHIVO")) == 0)
		{
			log_info(logger,"Leer  dice: ENCONTRE EL ARCHIVO A LEER");
			break;
		}
		i++;
		config_destroy(configArchivoActual);
	}
	bloqueAleer = punteroSeek /tamanioBloque;
	

	//Busco el bloque desde donde voy a leer usando los punteros del archivo
	if(bloqueAleer == 0)
	{
		moverPunteroAbloqueDelArchivo(bloques,configArchivoActual,bloqueAleer);
		//Me fijo si todo lo que voy a leer esta en un solo bloque
		if(bytesALeer < tamanioBloque || (bytesALeer) + (punteroSeek-bloqueAleer *tamanioBloque) < tamanioBloque )
		{
			infoDelArchivo= malloc(bytesALeer);
			fseek(bloques,punteroSeek,SEEK_CUR);
			fread(infoDelArchivo,bytesALeer,1,bloques);
			fclose(bloques);
			return infoDelArchivo;
		}
		//leo todo lo que puedo en el primer archivo y despues busco en el segundo
		else
		{
			infoDelArchivo= malloc(tamanioBloque-punteroSeek);
			fseek(bloques,punteroSeek,SEEK_CUR);
			fread(infoDelArchivo,tamanioBloque-punteroSeek,1,bloques);
			
			//Ahora me voy al bloque siguiente
			moverPunteroAbloqueDelArchivo(bloques,configArchivoActual,bloqueAleer + 1);

			infoDelArchivo2 = malloc(bytesALeer - (tamanioBloque-punteroSeek));
			fread(infoDelArchivo2,bytesALeer - (tamanioBloque-punteroSeek),1,bloques);
			punteroFinal = concatPunteros(infoDelArchivo , infoDelArchivo2 , tamanioBloque-punteroSeek , bytesALeer - (tamanioBloque-punteroSeek));
			free(infoDelArchivo);
			free(infoDelArchivo2);

		}
	}
	//no tengo que leer el bloque del puntero directo. Paso a buscar los demas bloques
	else
	{
		moverPunteroAbloqueDelArchivo(bloques,configArchivoActual,bloqueAleer);

		//Hay mas de un bloque para leer
		if(bytesALeer > tamanioBloque || (bytesALeer) + (punteroSeek-bloqueAleer *tamanioBloque) > tamanioBloque)
		{
			//Leo lo que puedo del primer bloque
			infoDelArchivo= malloc(tamanioBloque-(punteroSeek-bloqueAleer * tamanioBloque));			
			fseek(bloques,punteroSeek,SEEK_CUR);
			fread(infoDelArchivo,tamanioBloque-(punteroSeek-bloqueAleer * tamanioBloque),1,bloques);

			//Una vez leido todo lo que puedo del primer bloque vuelvo al bloque de punteros a buscar el que sigue
			moverPunteroAbloqueDelArchivo(bloques,configArchivoActual,bloqueAleer + 1);

			//Ahora leo lo que me falta leer del archivo
			infoDelArchivo2 = malloc(bytesALeer - tamanioBloque-(punteroSeek-bloqueAleer * tamanioBloque));
			fread(infoDelArchivo2,bytesALeer - tamanioBloque-(punteroSeek-bloqueAleer * tamanioBloque),1,bloques);
			concatPunteros(infoDelArchivo,infoDelArchivo2,tamanioBloque-(punteroSeek-bloqueAleer * tamanioBloque),bytesALeer - tamanioBloque-(punteroSeek-bloqueAleer * tamanioBloque));
			free(infoDelArchivo);
			free(infoDelArchivo2);
		}
		//Hay solo un bloque que leer
		else
		{
			infoDelArchivo = malloc(bytesALeer);
			fseek(bloques,punteroSeek,SEEK_CUR);
			fread(infoDelArchivo,bytesALeer,1,bloques);
			fclose(bloques);
			return infoDelArchivo;
		}
	}
	fclose(bloques);
	return punteroFinal;
}
void* concatPunteros(void* ptr1, void* ptr2, size_t size1, size_t size2) 
{
    char* resultado = malloc(size1 + size2); // Reserva memoria para el resultado

    memcpy(resultado, ptr1, size1); // Copia los bytes de ptr1 al resultado
    memcpy(resultado + size1, ptr2, size2); // Copia los bytes de ptr2 al resultado

    return resultado;
}

void moverPunteroABloquePunteros (FILE* bloques, t_config* configArchivoActual)
{
	log_info(logger,"Acceso Bloque - Archivo: %s - Bloque Archivo: bloque de punteros - Bloque File System %d",config_get_string_value(configArchivoActual,"NOMBRE_ARCHIVO"),config_get_int_value(configArchivoActual,"PUNTERO_INDIRECTO"));
	delay(config_get_int_value(config,"RETARDO_ACCESO_BLOQUE"));
	fseek(bloques,config_get_int_value(configArchivoActual,"PUNTERO_INDIRECTO")* config_get_int_value(superBloque,"BLOCK_SIZE"),SEEK_SET);
}
void moverPunteroAbloqueDelArchivo(FILE* bloques, t_config* configArchivoActual,int bloqueBuscado)
{
	uint32_t punteroAbloqueBuscado;
	
	if(bloqueBuscado == 0)
	{
		log_info(logger,"Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d",config_get_string_value(configArchivoActual,"NOMBRE_ARCHIVO"),bloqueBuscado,	config_get_int_value(configArchivoActual,"PUNTERO_DIRECTO"));
		fseek(bloques,config_get_int_value(configArchivoActual,"PUNTERO_DIRECTO") * config_get_int_value(superBloque,"BLOCK_SIZE") ,SEEK_SET);
		return;
	}
	else
	{
		moverPunteroABloquePunteros(bloques,configArchivoActual);
		fseek(bloques,sizeof(uint32_t) * (bloqueBuscado - 1),SEEK_CUR);
		//leo la posicion del bloque buscado bloque buscado
		fread(&punteroAbloqueBuscado,sizeof(uint32_t),1,bloques);
		//Me muevo a ese bloque
		log_info(logger,"Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d",config_get_string_value(configArchivoActual,"NOMBRE_ARCHIVO"),bloqueBuscado,	punteroAbloqueBuscado);
		fseek(bloques,punteroAbloqueBuscado * config_get_int_value(superBloque,"BLOCK_SIZE"),SEEK_SET);
		delay(config_get_int_value(config,"RETARDO_ACCESO_BLOQUE"));
		return;
	}
	

}
