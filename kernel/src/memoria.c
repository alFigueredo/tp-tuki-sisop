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
                log_error(logger,"La solicitud %s no puede ser realizada ya que ID=%d tiene un tamanio loco que impide que el tamanio_solicitado=%d sea gestionable", operacion,id_segmento,tamanio_solicitado);
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
