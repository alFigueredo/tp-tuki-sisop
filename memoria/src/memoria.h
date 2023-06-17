#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "server.h"
#include <operaciones.h>
#include <limits.h>

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

extern archivo_configuracion config_mem;

typedef struct
{
	uint32_t id_segmento;
	uint32_t tam_segmento; //BORRAR ???
	uint32_t direccion_base;
	uint32_t direccion_limite;

}segmento;

typedef enum {
	M_READ,
	M_WRITE
}inst_mem;

//VARIABLES
extern void* memoria_usuario;
extern t_list* tablas_segmentos;
extern t_list* lista_huecos_libre;
//int contadorSegmentos;

//FUNCIONES
void iniciar_memoria ();
void cargar_config (t_config* );
void terminar_memoria(t_log*, t_config*,int);
void manejo_instrucciones (t_list* , int* );
uint32_t leer_memoria (uint32_t );
void escribir_memoria (uint32_t , uint32_t );

#endif /* MEMORIA_H_ */
