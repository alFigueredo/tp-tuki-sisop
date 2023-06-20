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
typedef struct {
    t_list* segmentos;
} lista_tablas;

typedef enum {
	FIRST,
	BEST,
	WORST
}algoritmo_asignacion;

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
	unsigned int pid; //-1-> segmento 0 , -2-> hueco
	uint32_t id;
	//int libre;				//bool
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
extern t_list* tabla_segmentos_total;
extern t_list* huecos;
//extern t_list* listas_segmentos_procesos;
extern archivo_configuracion config_mem;
//int contadorSegmentos;

//FUNCIONES
algoritmo_asignacion cambiar_enum_algoritmo (char* );
t_list* obtener_segmentos_PID(int );
void crear_segmento(unsigned int , uint32_t );
void eliminar_segmento(uint32_t );
void iniciar_proceso(pcb* , int );
void finalizar_proceso(pcb* , int );
void agrupar_huecos(uint32_t , uint32_t );

void iniciar_memoria ();
void cargar_config (t_config* );
void iniciar_estructuras(pcb*, uint32_t);
int hay_espacio_disponible(int);
void first_fit (unsigned int, uint32_t);
void best_fit (unsigned int, uint32_t);
void worst_fit (unsigned int, uint32_t);
//void manejo_instrucciones (t_list* , int* );
uint32_t leer_memoria(int );
void escribir_memoria (int , uint32_t );
void terminar_memoria(t_log*, t_config*,int);
int comparar_segmentos(void* , void* );
void obtener_huecos_libres ();
uint32_t sumatoria_huecos();

#endif /* MEMORIA_H_ */
