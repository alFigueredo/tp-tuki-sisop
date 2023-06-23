#ifndef FILESYSTEMKERNEL_H_
#define FILESYSTEMKERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

typedef struct
{
	char *nombreDeArchivo;
	size_t puntero;
	t_queue* procesosBloqueados;
	struct Archivo *siguiente;
}Archivo;

extern t_list* archivosAbiertos;

int abrirArchivoKernel(pcb*, char*);
void cerrarArchivoKernel(pcb*, char*);
void buscarEnArchivo(pcb*, char*);
void truncarArchivo(pcb*, char*);



#endif /* FILESYSTEMKERNEL_H_ */
