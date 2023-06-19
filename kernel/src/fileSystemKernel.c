#include "fileSystemKernel.h"

int abriArchivoKernel(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	char* nombreArchivo = parsed[1];
	bool archivoExisteEnTabla = false;
	Archivo* archivoActual = NULL;
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
		archivoActual->puntero=0;
		list_add(archivosAbiertos, archivoActual);
		return 1;
	}
	else
	{
		//agregar el proceso a la lista de procesos bloqueados por este archivo
		list_add(archivoActual->procesosBloqueados, proceso);
		punteroOriginal=archivoActual->puntero;
		archivoActual->puntero=0;
		//agregar el archivo a la lista de archivos abiertos del proceso
		list_add(proceso->archivos_abiertos,archivoActual);
		archivoActual->puntero=punteroOriginal;
		exec_a_block();
		log_info(logger, "PID: %d - Bloqueado porque archivo %s ya esta abierto", proceso->pid, archivoActual->nombreDeArchivo);
		return 0;
	}

}

void cerrarArchivoKernel(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	char* nombreArchivo = parsed[1];
	bool archivoExisteEnTabla = false;
	Archivo* archivoActual = NULL;
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
}










