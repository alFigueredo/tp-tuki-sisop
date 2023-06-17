#include "fileSystemKernel.h"

void archivoKernel(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	char* operacion = parsed[0];
	bool archivoExisteEnTabla = false;
	Archivo* archivoActual = NULL;
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

	}
	else
	{
		if (recursoActual->instancias < 0)
		{
		    queue_push(recursoActual->procesosBloqueados, proceso);
		    exec_a_block();
		    log_info(logger, "PID: %d - Bloqueado por: %s", proceso->pid, rarchivoActual->nombreDeArchivo);
		}
	}

}
