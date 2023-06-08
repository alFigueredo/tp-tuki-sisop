#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include "server.h"

//STRUCTS
typedef struct
{
	char* ip_memoria;
	char* puerto_escucha;
	int   tam_memoria;
	int   tam_segmento_0;
	int   cant_segmentos;
	int   retardo_memoria;
	int   retardo_compactacion;
	char* algoritmo_asignacion;
} archivo_configuracion;

archivo_configuracion config_mem;

typedef struct
{
	char tipo;
	char* base;
	char* limite;
}segmento;

//VARIABLES
void* memoria_usuario;
t_list* lista_tabla_segmentos;
int contadorSegmentos;

//FUNCIONES
void terminar_programa(t_log*, t_config*);

#endif /* MEMORIA_H_ */
