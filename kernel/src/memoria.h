#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons/collections/list.h"
#include "commons/collections/queue.h"
#include "proceso.h"


bool gestionar_creacion(int id_segmento,int tamanio_solicitado,t_list* tabla_segmentos, t_paquete *paquete);
bool id_valido(int id, int tamanio);
void enviar_segmento(char* instruccion, t_list* tabla_segmentos);
void evaluar_respuesta(int resultado,int tipo_resultado);
pcb* buscar_proceso(int pid_buscado,t_list* lista);
void actualizar_cola(t_queue* cola,t_queue* cola_copia, t_list* lista_update);
void actualizar_tablas(t_list* lista_update);


#endif /* MEMORIA_H_ */

