#include "fileSystemKernel.h"

t_list* archivosAbiertos;
// int contadorDeEscrituraOLectura;

int abrirArchivoKernel(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	bool archivoExisteEnTabla = false;
	Archivo* archivoActual;
	int punteroOriginal;
	//verificar si el archivo esta en la tabla global.
	for (int i = 0; i < list_size(archivosAbiertos); i++)
	{
	    archivoActual = (Archivo*)list_get(archivosAbiertos, i);
	    if (strcmp(archivoActual->nombreDeArchivo, parsed[1]) == 0)
	    {
	        archivoExisteEnTabla = true;
	        break;
	    }
	}
	if (!archivoExisteEnTabla)
	{
		//agrega el archivo a la lista global
		archivoActual = malloc(sizeof(Archivo));
		archivoActual->nombreDeArchivo=malloc(strlen(parsed[1])+1);
		strcpy(archivoActual->nombreDeArchivo,parsed[1]);
		archivoActual->procesosBloqueados = queue_create();
		archivoActual->puntero=0;
		list_add(archivosAbiertos, archivoActual);
		//agregar el archivo a la lista de archivos abiertos del proceso
		list_add(proceso->archivos_abiertos,archivoActual);
		log_info(logger, "PID: %d - Abrir Archivo: %s", proceso->pid, parsed[1]);
		string_array_destroy(parsed);
		return 1;
	}
	else
	{
		//agregar el proceso a la lista de procesos bloqueados por este archivo
		// archivoActual->procesosBloqueados = queue_create();
		punteroOriginal=archivoActual->puntero;
		archivoActual->puntero=0;
		//agregar el archivo a la lista de archivos abiertos del proceso
		list_add(proceso->archivos_abiertos,archivoActual);
		queue_push(archivoActual->procesosBloqueados, proceso);
		archivoActual->puntero=punteroOriginal;
		exec_a_block();
		log_info(logger, "PID: %d - Bloqueado por: %s", proceso->pid, archivoActual->nombreDeArchivo);
		log_info(logger, "PID: %d - Bloqueado porque archivo %s ya esta abierto", proceso->pid, archivoActual->nombreDeArchivo);
		string_array_destroy(parsed);
		return 0;
	}
}

void cerrarArchivoKernel(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	pcb* procesoAux;
	Archivo* archivoActual;
	//verificar si el archivo esta en la tabla global y obtenerlo.
	for (int i = 0; i < list_size(archivosAbiertos); i++)
	{
	    archivoActual = (Archivo*)list_get(archivosAbiertos, i);
	    if (strcmp(archivoActual->nombreDeArchivo, parsed[1]) == 0)
	    {
	        break;
	    }
	}
	//saca el archivo de la lista de archivos del proceso

	for (int i = 0; i < list_size(proceso->archivos_abiertos); i++)
	{
	    archivoActual = (Archivo*)list_get(proceso->archivos_abiertos, i);
	    if (strcmp(archivoActual->nombreDeArchivo, parsed[1]) == 0)
         {
	        list_remove(proceso->archivos_abiertos, i);
         }
	}
	// list_remove_and_destroy_by_condition(proceso->archivos_abiertos,condicionParaBorrar(proceso->archivos_abiertos,parsed[1]),free);
	log_trace(logger, "TRACE: F_CLOSE");
	if(queue_is_empty(archivoActual->procesosBloqueados))
	{
		//saca el archivo de la lista global

		for (int i = 0; i < list_size(archivosAbiertos); i++)
		{
	    	archivoActual = (Archivo*)list_get(archivosAbiertos, i);
	    	if (strcmp(archivoActual->nombreDeArchivo, parsed[1]) == 0){
				free(archivoActual->nombreDeArchivo);
				queue_destroy(archivoActual->procesosBloqueados);
	        	list_remove_and_destroy_element(archivosAbiertos, i, free);
			}

		}
		// list_remove_and_destroy_by_condition(archivosAbiertos, condicionParaBorrar, free);
	}
	else
	{
		//saca de bloqueados al primer proceso de la cola de procesos bloqueados del archivo
		block_a_ready(queue_peek(archivoActual->procesosBloqueados));
		procesoAux=queue_pop(archivoActual->procesosBloqueados);
		log_info(logger, "PID: %d - Abrir Archivo: %s", procesoAux->pid, parsed[1]);

	}
	log_info(logger,"PID: %d - Cerrar Archivo: %s", proceso->pid, parsed[1]);
	string_array_destroy(parsed);

}

void buscarEnArchivo(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	int numero = atoi(parsed[2]);
	Archivo* archivoActual;
	for (int i = 0; i < list_size(archivosAbiertos); i++)
	{
	    archivoActual = (Archivo*)list_get(archivosAbiertos, i);
	    if (strcmp(archivoActual->nombreDeArchivo, parsed[1]) == 0)
	    {
	        break;
	    }
	}
	archivoActual->puntero = numero;
	log_info(logger, "PID: %d - Actualizar puntero Archivo: %s - Puntero: %d", proceso->pid, parsed[1], numero);
	enviar_pcb(conexion_cpu, proceso, EXEC);
	string_array_destroy(parsed);
}


Archivo *estoDevuelveUnArchivo(pcb* proceso, char* instruccion)
{
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	Archivo* archivoActual;
	for (int i = 0; i < list_size(archivosAbiertos); i++)
	{
	    archivoActual = (Archivo*)list_get(archivosAbiertos, i);
	    if (strcmp(archivoActual->nombreDeArchivo, parsed[1]) == 0)
	    {
	        break;
	    }
	}
	string_array_destroy(parsed);
	return archivoActual;
}

bool condicionParaBorrar(t_list* lista, char* nombre)
{
	Archivo* archivoActual;
	for (int i = 0; i < list_size(lista); i++)
	{
	    archivoActual = (Archivo*)list_get(lista, i);
	    if (strcmp(archivoActual->nombreDeArchivo, nombre) == 0)
	    {
	        return true;
	    }
	}
	return false;
}

void leer_archivo(pcb* proceso) {
	char* instruccion = list_get(proceso->instrucciones, proceso->program_counter-1);
	char** parsed = string_split(instruccion, " ");

	//esto deberia devolver el archivo que voy a usar
	Archivo* archivoQueUso = estoDevuelveUnArchivo(proceso, instruccion);

	char* numero = string_itoa(archivoQueUso->puntero);
	char* instruccionQueMando = string_from_format("%s %s", instruccion, numero);

	//PUEDO HACER ESTO????? X2
	t_instruccion* laCosaQueMando = generar_instruccion(proceso, instruccionQueMando);

	exec_a_block();
	sem_wait(sem_escrituraLectura);

	log_info(logger, "PID: %u - Leer Archivo: %s - Puntero: %s - Direccion Memoria %s - Tamanio %s", proceso->pid, parsed[1], numero, parsed[2], parsed[3]);
	enviar_instruccion(conexion_filesystem,laCosaQueMando,F_READ);

	string_array_destroy(parsed);
	free(numero);
	free(instruccionQueMando);
	destruir_instruccion(laCosaQueMando, 0);
}

void escribir_archivo(pcb* proceso) {
	char* instruccion = list_get(proceso->instrucciones, proceso->program_counter-1);
	char** parsed = string_split(instruccion, " ");

	//esto deberia devolver el archivo que voy a usar
	Archivo* archivoQueUso = estoDevuelveUnArchivo(proceso, instruccion);

	char* numero = string_itoa(archivoQueUso->puntero);

	//le meto el numero (como string) a la instruccion para mandarselo a file system
	char* instruccionQueMando = string_from_format("%s %s", instruccion, numero);

	t_instruccion* laCosaQueMando = generar_instruccion(proceso, instruccionQueMando);

	exec_a_block();
	sem_wait(sem_escrituraLectura);

	log_info(logger, "PID: %u - Escribir Archivo: %s - Puntero: %s - Direccion Memoria %s - Tamaño %s", proceso->pid, parsed[1], numero, parsed[2], parsed[3]);
	enviar_instruccion(conexion_filesystem,laCosaQueMando,F_WRITE);

	string_array_destroy(parsed);
	free(numero);
	free(instruccionQueMando);
	destruir_instruccion(laCosaQueMando, 0);
}
