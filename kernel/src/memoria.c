#include "memoria.h"


bool id_valido(int id, int tamanio){
	return id == 0? false : true;
}

bool gestionar_creacion(int tamanio_solicitado, Segmento* segmento, t_paquete *paquete){
    if(segmento->tam_segmento == 0 && tamanio_solicitado <= (segmento->direccion_limite - segmento->direccion_base)){ //tamanio_maximo = LIMITE - BASE 
        agregar_a_paquete(paquete,tamanio_solicitado, sizeof(tamanio_solicitado));
        enviar_paquete(paquete,conexion_memoria);
        return true;
    } 

    return false;

}

Segmento* buscar_segmento(t_list* tabla, int id_segmento){
    t_list_iterator* buscador = list_iterator_create(tabla);

    while(list_iterator_has_next(buscador)){
        Segmento* segmento = list_iterator_next(buscador);

        if(segmento->id_segmento == id_segmento && id_segmento != 0){
            list_iterator_destroy(buscador);
            return segmento;
        }   
    }
    list_iterator_destroy(buscador);
    return NULL;
}

void enviar_segmento(char* instruccion, t_list* tabla_segmentos){
	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
	char* operacion = parsed[0];
    Segmento* segmento = buscar_segmento(tabla_segmentos,atoi(parsed[1]));

	if(segmento != NULL){
        t_paquete *paquete = crear_paquete(operacion);
        agregar_a_paquete(paquete,segmento->id_segmento,sizeof(segmento->id_segmento));

        if(strcmp(operacion, "CREATE_SEGMENT") == 0){

            int tamanio_solicitado = atoi(parsed[2]); //El tamanio tambien se recibe de parametro

            if(!gestionar_creacion(tamanio_solicitado,segmento,paquete)){
                log_error(logger,"La solicitud %s no puede ser realizada ya que ID=%d tiene su tamanio_disponible=%d, impidiendo que el tamanio_solicitado=%d sea gestionable", operacion,segmento->id_segmento,segmento->tam_segmento,tamanio_solicitado);
            }
        }
        else{
            enviar_paquete(paquete,conexion_memoria);
        }
	}
	else{
		log_error(logger,"La solicitud %s no puede ser realizada ya que ID=%d no es un ID de segmento valido", operacion,atoi(parsed[1]));
	}
}


void evaluar_respuesta(int resultado,int tipo_resultado){
    switch(tipo_resultado){
        case 0:
            log_info(logger, "Se ha creado exitosamente el segmento con base=%d en memoria", resultado);
        case 1:
            log_error(logger,"Out of memory: No se encóntró espacio para el segmento id=%d en memoria, por lo que se finaliza el proceso en ejecucion", resultado);
            exec_a_exit();
        case 2:
            // enviar_mensaje("Confirmame, si puedo solicitar compactacion",conexion_filesystem,SE_PUEDE_COMPACTAR);
    }
}

bool actualizo_proceso(pcb* proceso,t_list* lista){
    t_list_iterator* buscador = list_iterator_create(lista);

    while(list_iterator_has_next(buscador)){
        t_instruction* proceso_actualizado = list_iterator_next(buscador);

        if(proceso_actualizado->pid == proceso->pid){
            proceso->tabla_segmentos = proceso_actualizado->tabla_segmentos;
            return true;
        }   
    }
    list_iterator_destroy(buscador);
    return false;
}


void actualizar_cola(t_queue* cola,t_queue* cola_copia, t_list* tablas_actualizadas){
    while(!queue_is_empty(cola)){
        if(actualizo_proceso(((pcb*)queue_peek(cola)),tablas_actualizadas)){
            log_info(logger, "El proceso PID=%d fue actualizado correctamente",((pcb*)queue_peek(cola))->pid);
        }
        else{
            log_error(logger, "El proceso PID=%d no fue encontrado en la lista con las tablas actualizadas",((pcb*)queue_peek(cola))->pid);
        }
        queue_push(cola_copia,((pcb*)queue_peek(cola))); 
        queue_pop(cola);
            
    }
    cola = cola_copia;
    queue_destroy(cola_copia);
}

void actualizar_tablas(t_list* tablas_actualizadas){
    t_queue* qready_copia = queue_create();  
    t_queue* qblock_copia = queue_create();  
    t_queue* qexec_copia = queue_create();  

    actualizar_cola(qready,qready_copia, tablas_actualizadas);
    actualizar_cola(qblock,qblock_copia, tablas_actualizadas);
    actualizar_cola(qexec,qexec_copia, tablas_actualizadas);

    list_destroy(tablas_actualizadas);

}