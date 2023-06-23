#include "fileSystemKernel.h"

t_list* archivosAbiertos;

int abriArchivoKernel(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	char* nombreArchivo = parsed[1];
	bool archivoExisteEnTabla = false;
	Archivo* archivoActual = malloc(sizeof(Archivo));
	int* punteroOriginal;
	//verificar si el archivo esta en la tabla global.
	for (int i = 0; i < list_size(archivosAbiertos); i++)
	{
	    archivoActual = list_get(archivosAbiertos, i);
	    if (strcmp(archivoActual->nombreDeArchivo, nombreArchivo) == 0)
	    {
	        archivoExisteEnTabla = true;
	        break;
	    }
	}
	if (!archivoExisteEnTabla)
	{
		//agrega el archivo a la lista global
		archivoActual->nombreDeArchivo = nombreArchivo;
		archivoActual->procesosBloqueados = queue_create();
		archivoActual->puntero=0;
		list_add(archivosAbiertos, archivoActual);
		enviar_pcb(conexion_cpu, proceso, EXEC);
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
	log_info(logger, "PID: %d - Abrir Archivo: %s", proceso->pid, nombreArchivo);
}

void cerrarArchivoKernel(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	char* nombreArchivo = parsed[1];
	bool archivoExisteEnTabla = false;
	Archivo* archivoActual = malloc(sizeof(Archivo));
	//verificar si el archivo esta en la tabla global y obtenerlo.
	for (int i = 0; i < list_size(archivosAbiertos); i++)
	{
	    archivoActual = list_get(archivosAbiertos, i);
	    if (strcmp(archivoActual->nombreDeArchivo, nombreArchivo) == 0)
	    {
	        archivoExisteEnTabla = true;
	        break;
	    }
	}
	//saca el archivo de la lista de archivos del proceso
	list_remove_and_destroy_by_condition(proceso->archivos_abiertos,strcmp(archivoActual->nombreDeArchivo, nombreArchivo) == 0,free);
	if(queue_is_empty(archivoActual->procesosBloqueados))
	{
		//saca el archivo de la lista global
		list_remove_and_destroy_by_condition(archivosAbiertos,strcmp(archivoActual->nombreDeArchivo, nombreArchivo) == 0,free);
	}
	else
	{
		//saca de bloqueados al primer proceso de la cola de procesos bloqueados del archivo
		block_a_ready(queue_peek(archivoActual->procesosBloqueados));
		queue_pop(archivoActual->procesosBloqueados);
	}
	log_info(logger,"PID: %d - Cerrar Archivo: %s", proceso->pid, nombreArchivo);

}

void buscarEnArchivo(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	char* nombreArchivo = parsed[1];
	int numero = atoi(parsed[2]);
	Archivo* archivoActual = malloc(sizeof(Archivo));
	for (int i = 0; i < list_size(archivosAbiertos); i++)
	{
	    archivoActual = list_get(archivosAbiertos, i);
	    if (strcmp(archivoActual->nombreDeArchivo, nombreArchivo) == 0)
	    {
	        break;
	    }
	}
	archivoActual->puntero = numero;
	enviar_pcb(conexion_cpu, proceso, EXEC);
	log_info(logger, "PID: %d - Actualizar puntero Archivo: %s - Puntero: %d", proceso->pid, nombreArchivo, numero);
}

void truncarArchivo(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	char* nombreArchivo = parsed[1];
	int numero = atoi(parsed[2]);
	Archivo* archivoActual = malloc(sizeof(Archivo));
	for (int i = 0; i < list_size(archivosAbiertos); i++)
	{
	    archivoActual = list_get(archivosAbiertos, i);
	    if (strcmp(archivoActual->nombreDeArchivo, nombreArchivo) == 0)
	    {
	        break;
	    }
	}



}









