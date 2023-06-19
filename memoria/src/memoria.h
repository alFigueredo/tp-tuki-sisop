#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "server.h"
#include <shared.h>
#include <limits.h>
#include <stdbool.h>

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
	algoritmo_asignacion algoritmo;
} archivo_configuracion;

extern archivo_configuracion config_mem;

typedef struct
{
	unsigned int pid;
	uint32_t id_segmento;
	uint32_t tam_segmento; //BORRAR ???
	uint32_t direccion_base;
	uint32_t direccion_limite;

}segmento;

typedef enum {
	M_READ,
	M_WRITE
}inst_mem;

typedef enum {
	FIRST,
	BEST,
	WORST
}algoritmo_asignacion;

//VARIABLES
extern void* memoria_usuario;
extern t_list* tabla_segmentos_total;
extern t_list* lista_huecos_libre;
//int contadorSegmentos;

//FUNCIONES
void iniciar_memoria ();
void cargar_config (t_config* );
void iniciar_estructuras(pcb*, uint32_t);
int hay_espacio_disponible(int);
void first_fit (unsigned int, uint32_t);
void best_fit (unsigned int, uint32_t);
void worst_fit (unsigned int, uint32_t);
//void manejo_instrucciones (t_list* , int* );
uint32_t leer_memoria(int direccion);
void escribir_memoria (uint32_t , uint32_t );
void terminar_memoria(t_log*, t_config*,int);

#endif /* MEMORIA_H_ */
