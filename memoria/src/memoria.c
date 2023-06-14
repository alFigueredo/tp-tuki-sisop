#include "memoria.h"

char* ip_memoria ;

archivo_configuracion config_mem;
void* memoria_usuario;
t_list* tablas_segmentos;
t_list* lista_huecos_libre;

//completar: !!!
//posibles cambios: ???

/*

 tema paqutes mensajes de sesrver
 */

int main(int argc, char** argv) {

	if (argc < 2) {
		return EXIT_FAILURE;
	}
	logger = iniciar_logger("./memoria.log", "Memoria");

	// CARGAR CONFIG
	t_config* config = iniciar_config (argv[1]);
	cargar_config (config);

	iniciar_memoria ();

	//int socket_servidor = -1;
	int socket_servidor = iniciar_servidor(config_mem.puerto_escucha);

	esperar_cliente(socket_servidor);

	terminar_memoria(logger, config , socket_servidor);
	return EXIT_SUCCESS;
}

//                                      INICIALIZACION DEL PROCESO

//config memoria
void cargar_config (t_config* archivo){
	config_mem.ip_memoria           = config_get_string_value (config, "IP_MEMORIA");
	config_mem.puerto_escucha       = config_get_string_value (config, "PUERTO_ESCUCHA");
	config_mem.tam_memoria          = config_get_int_value    (config, "TAM_MEMORIA");
	config_mem.tam_segmento_0       = config_get_int_value    (config, "TAM_SEGMENTO_0");
	config_mem.cant_segmentos       = config_get_int_value    (config, "CANT_SEGMENTOS");
	config_mem.retardo_memoria      = config_get_int_value    (config, "RETARDO_MEMORIA");
	config_mem.retardo_compactacion = config_get_int_value    (config, "RETARDO_COMPACTACION");
	config_mem.algoritmo_asignacion = config_get_string_value (config, "ALGORITMO_ASIGNACION");
}

void iniciar_memoria (){
	memoria_usuario    = malloc (config_mem.tam_memoria);

	tablas_segmentos   = list_create();
	lista_huecos_libre = list_create();

	segmento* segmento_0 = malloc (sizeof(segmento));
	segmento_0->id_segmento = 0;
	segmento_0->tam_segmento = config_mem.tam_segmento_0;
	segmento_0->direccion_base = 0;
	segmento_0->direccion_limite = config_mem.tam_segmento_0 - 1;


	list_add(tablas_segmentos, segmento_0);
	list_add(memoria_usuario, segmento_0);

	log_info (logger,"Memoria inicializada.");
}

void iniciar_estructuras(){

}

//                                    ALGORITMOS DE SEGMENTACION
void first_fit (uint32_t tam_segmento){ //Busca el primer hueco disponible desde el comienzo de memoria
	int segmento_asignado = -1;
	segmento* segmento_actual;
	segmento* segmento_siguiente;
	uint32_t espacio_libre;

	for (int i = 0; i < list_size(memoria_usuario); i++) {

		segmento_actual = list_get(memoria_usuario, i); //Recorre todos los segmentos

		if (i+1 < config_mem.tam_memoria) { //Ve si hay otro segmento
			segmento_siguiente = list_get(memoria_usuario, i + 1);

			espacio_libre = segmento_siguiente->direccion_base - (segmento_actual->direccion_limite + 1);

			if (espacio_libre >= tam_segmento) {
				segmento_asignado = segmento_actual->id_segmento + 1; // ID del nuevo segmento
				segmento* nuevo_segmento = malloc(sizeof(segmento));
			    nuevo_segmento->id_segmento = segmento_asignado;
			    nuevo_segmento->tam_segmento = tam_segmento;
			    nuevo_segmento->direccion_base = segmento_actual->direccion_limite + 1;
			    nuevo_segmento->direccion_limite = nuevo_segmento->direccion_base + tam_segmento - 1;
			    list_add_in_index(memoria_usuario, i + 1, nuevo_segmento);
			    break;
			}
		}
	}
}

void best_fit (uint32_t tam_segmento){ //Busca el hueco mas chico donde entre el proceso
	int segmento_asignado = -1;
	int mejor_ajuste = INT_MAX;
	segmento* segmento_actual;
	segmento* segmento_siguiente;
	int espacio_libre;
	segmento* nuevo_segmento;
	uint32_t nueva_dir_base;

	    for (int i = 0; i < list_size(memoria_usuario); i++) {
	        segmento_actual = list_get(memoria_usuario, i);

	        if (i + 1 < list_size(memoria_usuario)) {
	            segmento_siguiente = list_get(memoria_usuario, i + 1);
	            espacio_libre = segmento_siguiente->direccion_base - (segmento_actual->direccion_limite + 1);

	            if (espacio_libre >= tam_segmento && espacio_libre < mejor_ajuste) {
	                segmento_asignado = segmento_actual->id_segmento + 1;
	                mejor_ajuste = espacio_libre;
	                nueva_dir_base = segmento_actual->direccion_limite +1;
	            }
	        }
	    }

	    if (segmento_asignado != -1) {
	        nuevo_segmento = malloc(sizeof(segmento));
	        nuevo_segmento->id_segmento = segmento_asignado;
	        nuevo_segmento->tam_segmento = tam_segmento;
	        nuevo_segmento->direccion_base = nueva_dir_base;
	        nuevo_segmento->direccion_limite = nuevo_segmento->direccion_base + tam_segmento - 1;

	        // Insertar el nuevo segmento en la lista de memoria después del segmento anterior al segmento asignado
	        list_add_in_index(memoria_usuario, segmento_asignado, nuevo_segmento);
	    }
}

//                                    ACCESO A ESPACIO USUARIO


void manejo_instrucciones (t_list* instrucciones, int* socket_cliente){
	inst_mem tipo_instruccion = *(inst_mem *) list_get (instrucciones,0);
	uint32_t dir_dada = *(uint32_t*) list_get(instrucciones,1);                                                             //Direccion fisica dada por CPU o FS

	uint32_t valor;
	uint32_t nuevo_valor;
	//int boolEsc;


	/*  Tanto CPU como File System pueden, dada una dirección física, solicitar accesos al espacio
		 de usuario de Memoria.
		 Para simular la realidad y la velocidad de los accesos a Memoria, cada acceso al espacio de usuario tendrá un tiempo de espera en milisegundos definido por archivo de configuración*/


		switch (tipo_instruccion){
		case M_READ:                                                                                 //Ante un pedido de lectura, devolver el valor que se encuentra en la posicion pedida.
			usleep (config_mem.retardo_memoria * 1000);                                            //Tiempo de espera
				//Microsegundos = Milisegundos * 1000
			valor = leer_memoria (dir_dada);                                               		   //Busca y retorna el valor en la direccion de memoria dada
			log_info (logger, "Se leyo el valor %d en la posicion %d", valor, dir_dada);

			//AGREGAR TD LO DE PAQUETES !!!

			break;

		case M_WRITE:                                                                                // Ante un pedido de escritura, escribir lo indicado en la posición pedida y responder un mensaje de ‘OK’.
			nuevo_valor = *(uint32_t*) list_get(instrucciones,2);

			usleep (config_mem.retardo_memoria * 1000);
			escribir_memoria (nuevo_valor, dir_dada);
			//responder con un msj OK !!!
			log_info (logger, "Se escribio el valor %d en la posicion %d", nuevo_valor, dir_dada);
			break;

		default:
					log_warning(logger,"Operacion desconocida \n");
					break;
		}

}

//no dividir funcion ???
uint32_t leer_memoria (uint32_t direccion){
	uint32_t valorLeido;

	//MUTEX SC !!!
	memcpy (&valorLeido, memoria_usuario + direccion, sizeof (uint32_t ));
	//MUTEX SC !!!

	return valorLeido;
}

void escribir_memoria (uint32_t valor, uint32_t direccion) {
		//MUTEX SC !!!
		memcpy (memoria_usuario + direccion, &valor, sizeof(uint32_t));
		//MUTEX SC !!!
}


void terminar_memoria(t_log* logger, t_config* config, int socket)
{
	log_info(logger, "Finalizando memoria");
	if (logger != NULL) {
		log_destroy(logger);
	}
	if (config != NULL) {
		config_destroy(config);
	}

	liberar_servidor (socket);
}
