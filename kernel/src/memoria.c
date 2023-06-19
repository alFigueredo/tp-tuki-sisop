#include "memoria.h"


bool id_valido(int id, int tamanio){
	return id == 0 || id >tamanio? false : true;
}

bool gestionar_creacion(int id_segmento,int tamanio_solicitado,t_list* tabla_segmentos, t_paquete *paquete){
    if(1 == 0 && tamanio_solicitado <= 2){ // 1 es tamanio_actual del segmento y 2 tamanio_maximo = LIMITE - BASE - TAMANIO_ACTUAL
        agregar_a_paquete(paquete,tamanio_solicitado, sizeof(tamanio_solicitado));
        enviar_paquete(paquete,conexion_memoria);
        return true;
    } 

    return false;

}

void enviar_segmento(char* instruccion, t_list* tabla_segmentos){
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	char* operacion = parsed[0];
    int id_segmento = atoi(parsed[1]);


    int tamanio_tabla;


	if(id_valido(id_segmento, tamanio_tabla)){
        t_paquete *paquete = crear_paquete(operacion);
        agregar_a_paquete(paquete,id_segmento,sizeof(id_segmento));

        if(strcmp(operacion, "CREATE_SEGMENT") == 0){

            int tamanio_solicitado = atoi(parsed[2]);

            if(!gestionar_creacion(id_segmento,tamanio_solicitado,tabla_segmentos,paquete)){
                log_error(logger,"La solicitud %s no puede ser realizada ya que ID=%d tiene un tamanio_loco que impide que el tamanio_solicitado=%d sea gestionable", operacion,id_segmento,tamanio_solicitado);
            }
        }
        else{
            enviar_paquete(paquete,conexion_memoria);
        }
	}
	else{
		log_error(logger,"La solicitud %s no puede ser realizada ya que ID=%d no es un ID de segmento valido", operacion,id_segmento);
	}
}


void evaluar_respuesta(int resultado,int tipo_resultado){
    switch(tipo_resultado){
        case 1:
            log_error(logger,"Out of memory: No se encóntró espacio para el segmento de id=%d en memoria, por lo que se finaliza el proceso en ejecucion", resultado);
            exec_a_exit();
        case 2:
            enviar_mensaje("Hay ejecuciones en curso?",conexion_filesystem,EXEC);
    }
}

pcb* buscar_proceso(int pid_buscado,t_list* lista){
    t_list_iterator* buscador = list_iterator_create(lista);

    while(list_iterator_has_next(buscador)){
        pcb* proceso = list_iterator_next(buscador);

        if(proceso->pid == pid_buscado){
            list_iterator_destroy(buscador);
            return proceso;
        }   
    }
    list_iterator_destroy(buscador);
    return NULL;
}


void actualizar_cola(t_queue* cola,t_queue* cola_copia, t_list* lista_update){
    while(!queue_is_empty(cola)){
        pcb* proceso_actualizado = buscar_proceso(((pcb*)queue_peek(cola))->pid,lista_update);
        if(proceso_actualizado!=NULL){
            queue_push(cola_copia,proceso_actualizado);
            queue_pop(cola);
        }
        else{
            log_error(logger, "El proceso PID=%d no fue encontrado en la lista con los PCB actualizados",((pcb*)queue_peek(cola))->pid);
        }
            
    }
    cola = cola_copia;
}

void actualizar_tablas(t_list* lista_update){
    t_queue* qready_copia = queue_create();  
    t_queue* qblock_copia = queue_create();  
    t_queue* qexec_copia = queue_create();  

    actualizar_cola(&qready,qready_copia, lista_update);
    actualizar_cola(&qblock,qblock_copia, lista_update);
    actualizar_cola(&qexec,qexec_copia, lista_update);

}