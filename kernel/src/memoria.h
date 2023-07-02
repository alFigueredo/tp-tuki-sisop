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


bool gestionar_creacion(int, Segmento*, t_paquete*);
void enviar_segmento(char*, t_list*);
void evaluar_respuesta(int,int);
bool actualizo_proceso(pcb* proceso, t_list* lista);
void actualizar_cola(t_queue*,t_queue*, t_list*);
void actualizar_tablas(t_list*);




#endif /* MEMORIA_H_ */

