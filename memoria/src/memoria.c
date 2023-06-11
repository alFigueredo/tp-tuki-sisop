#include "memoria.h"

char* ip_memoria ;

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

	terminar_programa(logger, config , socket_servidor);
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
}

//                                    ACCESO A ESPACIO USUARIO

void manejo_instrucciones (t_list* instrucciones, int* socket_cliente){
	cod_op_inst tipo_instruccion = *(cod_op_inst *) list_get (instrucciones,0);
	uint32_t dir_dada = *(uint32_t*) list_get(instrucciones,1);                                                             //Direccion fisica dada por CPU o FS

	uint32_t valor;
	uint32_t nuevo_valor;
	//int boolEsc;

	/*
	  Tanto CPU como File System pueden, dada una dirección física, solicitar accesos al espacio
		 de usuario de Memoria.
		 Para simular la realidad y la velocidad de los accesos a Memoria, cada acceso al espacio de usuario tendrá un tiempo de espera en milisegundos definido por archivo de configuración
	 */

		switch (tipo_instruccion){
		case READ:                                                                                 //Ante un pedido de lectura, devolver el valor que se encuentra en la posicion pedida.
			usleep (config_mem.retardo_memoria * 1000);                                            //Tiempo de espera
				//Microsegundos = Milisegundos * 1000
			valor = leer_memoria (dir_dada);                                               		   //Busca y retorna el valor en la direccion de memoria dada
			log_info (logger, "Se leyo el valor %d en la posicion %d", valor, dir_dada);

			//AGREGAR TD LO DE PAQUETES !!!

			break;

		case WRITE:                                                                                // Ante un pedido de escritura, escribir lo indicado en la posición pedida y responder un mensaje de ‘OK’.
			nuevo_valor = *(uint32_t*) list_get(instrucciones,2);

			usleep (config_mem.retardo_memoria * 1000);
			escribir_memoria (nuevo_valor, dir_dada);
			//responder con un msj OK !!!
			log_info (logger, "Se escribio el valor %d en la posicion %d", nuevo_valor, dir_dada);

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
