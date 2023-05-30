#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons/config.h"
#include "commons/collections/list.h"
#include "proceso.h"


typedef struct Recurso {
    char nombre[20];
    int instancias;
    tlist* procesosBloqueados;
    struct Recurso *siguiente;
} Recurso;

extern tlist* recursos;

t_list leerRecursos(t_config *config);

void procesar_contexto_ejecucion(lista);
