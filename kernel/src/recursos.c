#include "recursos.h"


t_list* leerRecursos(t_config *config) {
    t_list* listaRecursos = list_create();

    char** recursosConfig =  config_get_array_value(config, "RECURSOS");
    char** instanciasConfig = config_get_array_value(config, "INSTANCIAS_RECURSOS");

    int numRecursos = sizeof(recursosConfig) / sizeof(recursosConfig[0]);

    for(int i = 0; i<numRecursos ;i++) {
		Recurso* nuevoRecurso = malloc(sizeof(Recurso));
		strcpy(nuevoRecurso->nombre, recursosConfig[i]);
		nuevoRecurso->instancias = instanciasConfig[i];
		nuevoRecurso->siguiente = NULL;
		nuevoRecurso->procesosBloqueados = queue_create();
		list_add(listaRecursos, nuevoRecurso);
	}

	return listaRecursos;
}



void manejo_recursos(pcb* proceso, char* instruccion) {

    char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual

    char* operacion = parsed[0];

    bool recursoExiste = false;

    Recurso* recursoActual = NULL;

    for (int i = 0; i < list_size(recursos); i++) {
        recursoActual = list_get(recursos, i);
        if (strcmp(recursoActual->nombre, parsed[1]) == 0) {
            recursoExiste = true;
            break;
        }
    }

    if (!recursoExiste) {
    	exec_a_exit();
    } else {
        if (strcmp(operacion, "WAIT") == 0) {
            // Procesar operación WAIT
            recursoActual->instancias--;
            if (recursoActual->instancias < 0) {
            	queue_push(recursoActual->procesosBloqueados, proceso);
            	exec_a_block();
            }
            else{
                enviar_pcb(conexion_cpu, proceso, EXEC);
            }
        } else if (strcmp(operacion, "SIGNAL") == 0) {
            // Procesar operación SIGNAL
            recursoActual->instancias++;
            if (recursoActual->instancias <= 0) {
                block_a_ready(queue_peek(recursoActual->procesosBloqueados));
                queue_pop(recursoActual->procesosBloqueados);
                // Desbloquear primer proceso en la cola de bloqueados del recurso (si corresponde)
            }
            enviar_pcb(conexion_cpu, proceso, EXEC);
        }
    }
}





