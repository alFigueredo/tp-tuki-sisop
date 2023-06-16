#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons/collections/list.h"
#include "proceso.h"


bool gestionar_creacion(int id_segmento,int tamanio_solicitado,t_list* tabla_segmentos, t_paquete *paquete);
bool id_valido(int id, int tamanio);
void enviar_segmento(char* instruccion, t_list* tabla_segmentos);

#endif /* MEMORIA_H_ */

