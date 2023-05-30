#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons/config.h"
#include "commons/collections/list.h"

typedef struct Recurso {
    char nombre[100];
    int instancias;
    struct Recurso *siguiente;
} Recurso;

t_list leerRecursos(t_config *config);

void procesar_contexto_ejecucion(lista);
