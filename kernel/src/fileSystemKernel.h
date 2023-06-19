#ifndef FILESYSTEMKERNEL_H_
#define FILESYSTEMKERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>

typedef struct
{
	char *nombreDeArchivo;
	int* puntero;
	t_queue* procesosBloqueados;
	struct Archivo *siguiente;
}Archivo;

extern t_list* archivosAbiertos;

int abrirArchivoKernel(pcb*, char*);
void cerrarArchivoKernel(pcb*, char*);





#endif /* FILESYSTEMKERNEL_H_ */
