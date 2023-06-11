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
	char* base;
	char* limite;

}segmento;

//VARIABLES
void* memoria_usuario;
t_list* tablas_segmentos;
t_list* lista_huecos_libre;
//int contadorSegmentos;

//FUNCIONES
void iniciar_memoria ();
void cargar_config (t_config* );
void terminar_memoria(t_log*, t_config*,int);
void manejo_instrucciones (t_list* , int* );
uint32_t leer_memoria (uint32_t );
void escribir_memoria (uint32_t , uint32_t );

#endif /* MEMORIA_H_ */
