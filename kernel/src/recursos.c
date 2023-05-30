#include "recursos.h"


t_list leerRecursos(t_config *config) {
    t_list *listaRecursos = list_create();
    listaRecursos-> procesosBloqueados = list_create();

    char *recursosString = config_get_string_value(config, "RECURSOS");
    char **recursos = string_get_string_as_array(recursosString);

    char *instanciasString = config_get_string_value(config, "INSTANCIAS_RECURSOS");
    char **instancias = string_get_string_as_array(instanciasString);

    int numRecursos = config_keys_amount(config, "RECURSOS"); // Cantidad de recursos

    for (int i = 0; i < numRecursos; i++) {
        Recurso *nuevoRecurso = malloc(sizeof(Recurso));
        strcpy(nuevoRecurso->nombre, recursos[i]);
        nuevoRecurso->instancias = atoi(instancias[i]);
        nuevoRecurso->siguiente = NULL;

        list_add(listaRecursos, nuevoRecurso);
    }

    string_iterate_lines(recursos, free);
    string_iterate_lines(instancias, free);
    free(recursos);
    free(instancias);

    return listaRecursos;
}


void procesar_contexto_ejecucion(pcb* contexto) {
    // Obtener el contexto actual de ejecución
    pcb* contexto_ejecucion = (t_contexto_ejecucion*)list_get(contexto, 0);

    char* nombreRecurso = contexto_ejecucion->recurso;
    char* operacion = contexto_ejecucion->operacion;

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
            	list.add(recursos->procesosBloqueados, proceso);
            	exec_a_block();
            }
        } else if (strcmp(operacion, "SIGNAL") == 0) {
            // Procesar operación SIGNAL
            recurso->instancias++;
            if (recurso->instancias < 0) {
                list.remove(recursos->procesosBloqueados,proceso);
                exec_a_ready();
            }
            // Desbloquear primer proceso en la cola de bloqueados del recurso (si corresponde)
        }
    }
}





