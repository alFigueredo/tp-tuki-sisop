#include "fileSystemKernel.h"

t_list* archivosAbiertos;
int contadorDeEscrituraOLectura;

int abriArchivoKernel(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	bool archivoExisteEnTabla = false;
	Archivo* archivoActual = malloc(sizeof(Archivo));
	int punteroOriginal;
	//verificar si el archivo esta en la tabla global.
	for (int i = 0; i < list_size(archivosAbiertos); i++)
	{
	    archivoActual = list_get(archivosAbiertos, i);
	    if (strcmp(archivoActual->nombreDeArchivo, parsed[1]) == 0)
	    {
	        archivoExisteEnTabla = true;
	        break;
	    }
	}
	if (!archivoExisteEnTabla)
	{
		//agrega el archivo a la lista global
		archivoActual->nombreDeArchivo=malloc(sizeof((char)*(strlen(parsed[1]))));
		strcpy(archivoActual->nombreDeArchivo,parsed[1]);
		archivoActual->procesosBloqueados = queue_create();
		archivoActual->puntero=0;
		list_add(archivosAbiertos, archivoActual);
		//agregar el archivo a la lista de archivos abiertos del proceso
		list_add(proceso->archivos_abiertos,archivoActual);
		enviar_pcb(conexion_cpu, proceso, EXEC);
		log_info(logger, "PID: %d - Abrir Archivo: %s", proceso->pid, parsed[1]);
		return 1;
	}
	else
	{
		//agregar el proceso a la lista de procesos bloqueados por este archivo
		archivoActual->procesosBloqueados = queue_create();
		punteroOriginal=archivoActual->puntero;
		archivoActual->puntero=0;
		//agregar el archivo a la lista de archivos abiertos del proceso
		list_add(proceso->archivos_abiertos,archivoActual);
		archivoActual->puntero=punteroOriginal;
		exec_a_block();
		log_warning(logger, "PID: %d - Bloqueado porque archivo %s ya esta abierto", proceso->pid, archivoActual->nombreDeArchivo);
		return 0;
	}
}

void cerrarArchivoKernel(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	bool archivoExisteEnTabla = false;
	pcb* procesoAux;
	Archivo* archivoActual = malloc(sizeof(Archivo));
	//verificar si el archivo esta en la tabla global y obtenerlo.
	for (int i = 0; i < list_size(archivosAbiertos); i++)
	{
	    archivoActual = list_get(archivosAbiertos, i);
	    if (strcmp(archivoActual->nombreDeArchivo, parsed[1]) == 0)
	    {
	        archivoExisteEnTabla = true;
	        break;
	    }
	}
	//saca el archivo de la lista de archivos del proceso
	list_remove_and_destroy_by_condition(proceso->archivos_abiertos,condicionParaBorrar(proceso->archivos_abiertos,parsed[1]),free);
	if(queue_is_empty(archivoActual->procesosBloqueados))
	{
		//saca el archivo de la lista global
		list_remove_and_destroy_by_condition(archivosAbiertos,condicionParaBorrar(proceso->archivos_abiertos,parsed[1]),free);
	}
	else
	{
		//saca de bloqueados al primer proceso de la cola de procesos bloqueados del archivo
		block_a_ready(queue_peek(archivoActual->procesosBloqueados));
		procesoAux=queue_pop(archivoActual->procesosBloqueados);
		log_info(logger, "PID: %d - Abrir Archivo: %s", procesoAux->pid, parsed[1]);

	}
	log_info(logger,"PID: %d - Cerrar Archivo: %s", proceso->pid, parsed[1]);

}

void buscarEnArchivo(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	int numero = atoi(parsed[2]);
	Archivo* archivoActual = malloc(sizeof(Archivo));
	for (int i = 0; i < list_size(archivosAbiertos); i++)
	{
	    archivoActual = list_get(archivosAbiertos, i);
	    if (strcmp(archivoActual->nombreDeArchivo, parsed[1]) == 0)
	    {
	        break;
	    }
	}
	archivoActual->puntero = numero;
	enviar_pcb(conexion_cpu, proceso, EXEC);
	log_info(logger, "PID: %d - Actualizar puntero Archivo: %s - Puntero: %d", proceso->pid, parsed[1], numero);
}


Archivo *estoDevuelveUnArchivo(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	Archivo* archivoActual = malloc(sizeof(Archivo));
	for (int i = 0; i < list_size(archivosAbiertos); i++)
	{
	    archivoActual = list_get(archivosAbiertos, i);
	    if (strcmp(archivoActual->nombreDeArchivo, parsed[1]) == 0)
	    {
	        break;
	    }
	}
	return archivoActual;
}

bool condicionParaBorrar(t_list* lista, char* nombre)
{
	for (int i = 0; i < list_size(lista); i++)
	{
	    archivoActual = list_get(lista, i);
	    if (strcmp(archivoActual->nombreDeArchivo, nombre) == 0)
	    {
	        return 1;
	    }
	}
}






