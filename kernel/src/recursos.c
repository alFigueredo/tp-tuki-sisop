#include "recursos.h"


t_list* leerRecursos(t_config *config) {
    t_list* listaRecursos = list_create();

    char** recursosConfig =  config_get_array_value(config, "RECURSOS");
    char** instanciasConfig = config_get_array_value(config, "INSTANCIAS_RECURSOS");

    int numRecursos = sizeof(recursosConfig) / sizeof(recursosConfig[0]);

    for(int i = 0; i<numRecursos ;i++) {
		Recurso* nuevoRecurso = malloc(sizeof(Recurso));
		strcpy(nuevoRecurso->nombre, recursoToken);
		nuevoRecurso->instancias = atoi(instanciaToken);
		nuevoRecurso->siguiente = NULL;
		nuevoRecurso->procesosBloqueados = queue_create();
		list_add(listaRecursos, nuevoRecurso);
	}

	return listaRecursos;
}



void procesar_contexto_ejecucion(tlist* contexto) {
    // Obtener el contexto actual de ejecución
    proceso = (tlist*)list_get(contexto, 0);

    char** parsed = string_split(list_get(proceso->instrucciones, proceso->program_counter), " "); //Partes de la instruccion actual

    char* operacion = parsed[0];
    char* recurso = parsed[1];

    Recurso* recurso = NULL;
    for (int i = 0; i < list_size(recursos); i++) {
        Recurso* recursoActual = list_get(recursos, i);
        if (strcmp(recursoActual->nombre, nombreRecurso) == 0) {
            recurso = recursoActual;
            break;
        }
    }

    if (recurso == NULL) {
    	exec_a_exit();
    } else {
        if (strcmp(operacion, "WAIT") == 0) {
            // Procesar operación WAIT
            recurso->instancias--;
            if (recurso->instancias < 0) {
            	queue.push(recursos->procesosBloqueados, proceso);
            	exec_a_block();
            }
        } else if (strcmp(operacion, "SIGNAL") == 0) {
            // Procesar operación SIGNAL
            recurso->instancias++;
            if (recurso->instancias < 0) {
                queue.pop(recursos->procesosBloqueados,proceso);
                exec_a_ready();
            }
            // Desbloquear primer proceso en la cola de bloqueados del recurso (si corresponde)
        }
    }
}





