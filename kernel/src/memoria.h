#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons/collections/list.h"
#include "commons/collections/queue.h"
#include "proceso.h"

typedef struct{
    int id_segmento;
    int tam_segmento;
    int direccion_base;
	int direccion_limite;
}Segmento;


bool gestionar_creacion(int tamanio_solicitado, Segmento* segmento, t_paquete *paquete);
bool id_valido(int id, int tamanio);
void enviar_segmento(char* instruccion, t_list* tabla_segmentos);
void evaluar_respuesta(int resultado,int tipo_resultado);
bool actualizo_proceso(pcb* proceso,t_list* tablas_actualizadas);
void actualizar_cola(t_queue* cola,t_queue* cola_copia, t_list* tablas_actualizadas);
void actualizar_tablas(t_list* tablas_actualizadas);




#endif /* MEMORIA_H_ */

