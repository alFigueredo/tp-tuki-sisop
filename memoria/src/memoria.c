#include "memoria.h"

char *ip_memoria;

archivo_configuracion config_mem;
void* memoria_usuario;
t_list* tabla_segmentos_total;
t_list* huecos;

// completar: !!!
// posibles cambios: ???


int main(int argc, char **argv)
{

	if (argc < 2)
	{
		return EXIT_FAILURE;
	}
	logger = iniciar_logger("./memoria.log", "Memoria");

	// CARGAR CONFIG
	t_config *config = iniciar_config(argv[1]);
	cargar_config(config);

	iniciar_memoria();

	// SERIVIDOR
	int socket_servidor = iniciar_servidor(config_mem.puerto_escucha);
	esperar_cliente(socket_servidor);

	//pruebas


	//terminar_memoria(logger, config, socket_servidor);
	return EXIT_SUCCESS;
}

//-------------------PROCESOS------------------------------------------------------------------------------------------------------------------------------------------------------------
//Crea los segmentos del proceso. La cantidad de segmentos esta dada por el config.
void iniciar_proceso(pcb* pcb_proceso, int tamanio)
{
	//t_list* segmentos;

	for (int var = 0; var < config_mem.cant_segmentos ; ++var)
	{
		crear_segmento (pcb_proceso->pid, tamanio);
	}


	//segmentos = obtener_segmentos_PID(pcb_proceso->pid);

	//MANDAR TABLA A NO SE QUI9EN !!!

	log_info (logger, "Creacion de proceso PID: %u",pcb_proceso->pid);
}

//Elimina los segmentos del proceso. Filtra la lista de segmentos y los elimina de la lista de segmentos.
void finalizar_proceso(pcb* pcb_proceso, int tamanio)
{
	t_list* segmentos_proc = obtener_segmentos_PID (pcb_proceso->pid); 									//Lista filtrada
	segmento* seg;

	//FALTA ELIMINAR DE MEMORIA USUARIO !!!

	for (int var = 0; var < list_size(segmentos_proc); ++var)											//Recorrre la lista filtrada
		{
			seg = list_get(segmentos_proc, var);														//Toma el segmento actual del for
			eliminar_segmento (pcb_proceso->pid,seg->id);												//Elimina el segmento de la lista de segmentos total.
		}

	list_destroy_and_destroy_elements(segmentos_proc, free);
	log_info (logger, "Eliminacion de proceso PID: %u",pcb_proceso->pid);
}

//Funcion que recorre la lista y filtra por pid
t_list* obtener_segmentos_PID(unsigned int pid)
{
    t_list* segmentosPorPID = list_create();
    segmento* seg;

    for (int i = 0; i < list_size(tabla_segmentos_total); i++)											//Recorre la lista de segmentos
    {
        seg = list_get(tabla_segmentos_total, i);														//Toma un segmento
        if (seg->pid == pid) {																			//Ve si pertenece al proceso
            list_add(segmentosPorPID, seg);																//Si pertenece, lo agrega a la lista de segmentos del proceso
        }
    }

    return segmentosPorPID;
}

//-------------------INICIALIZACION DE MEMORIA------------------------------------------------------------------------------------------------------------------------------------------------------------
//Se le asignan a los elementos de un struct global la informacion del config
void cargar_config(t_config *archivo)
{
	char* algoritmo = config_get_string_value(config, "ALGORITMO_ASIGNACION");
	char* primera_letra = string_substring_until(algoritmo, 1);
	algoritmo_asignacion alg = cambiar_enum_algoritmo (primera_letra);									//Quiero que el algoritmo de asignacion sea un enum porque uso un switch en crear segmento.

	config_mem.ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	config_mem.puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	config_mem.tam_memoria = config_get_int_value(config, "TAM_MEMORIA");
	config_mem.tam_segmento_0 = config_get_int_value(config, "TAM_SEGMENTO_0");
	config_mem.cant_segmentos = config_get_int_value(config, "CANT_SEGMENTOS");
	config_mem.retardo_memoria = config_get_int_value(config, "RETARDO_MEMORIA");
	config_mem.retardo_compactacion = config_get_int_value(config, "RETARDO_COMPACTACION");
	config_mem.algoritmo = alg;
}

//Cambia el string del algoritmo de asignacion del config a un enum propio
algoritmo_asignacion cambiar_enum_algoritmo (char* letra)
{
	algoritmo_asignacion algoritmo;

    if (string_equals_ignore_case(letra, "F")) {
    	algoritmo = FIRST;
    }
    else if (string_equals_ignore_case(letra, "B")) {
    	algoritmo = BEST;
    }
    else if (string_equals_ignore_case(letra, "W")) {
    	algoritmo = WORST;
    }

    return algoritmo;
}

void iniciar_memoria()
{
	memoria_usuario = malloc(config_mem.tam_memoria);

	tabla_segmentos_total = list_create();
	huecos = list_create();

	segmento *segmento_0 = (segmento *)malloc(sizeof(segmento));										//Segmento 0 compartido por todos los procesos
	segmento_0->pid = -1;
	segmento_0->id = 0;
	segmento_0->tam_segmento = config_mem.tam_segmento_0;
	segmento_0->direccion_base = 0;
	segmento_0->direccion_limite = config_mem.tam_segmento_0 - 1;

	list_add(tabla_segmentos_total, segmento_0);

	log_info(logger, "Memoria inicializada.");
}

//-------------------MANEJO DE SEGMENTOS-----------------------------------------------------------------------------------
//Crea UN segmento segun el algoritmo de asignacion del config si hay espacio en memoria. Si no hay espacio en memoria, solicita una compactacion o le informa al kernel que no hay espacio disponible
void crear_segmento(unsigned int pid, uint32_t tamanio_seg)
{
	uint32_t sumatoria;

	if (hay_espacio_disponible(tamanio_seg))															//Primero se fija si hay espacio disponible en memoria
	{
			// mutex
			switch (config_mem.algoritmo)
			{
				case FIRST:
					first_fit(pid, tamanio_seg);
					break;
				case BEST:
					best_fit(pid, tamanio_seg);
					break;
				case WORST:
					worst_fit(pid, tamanio_seg);
					break;
			}
			// mutex
		}
	else																								//No hay espacio disponible en memoria,
	{
		sumatoria = sumatoria_huecos();																			//suma el espacio de los huecos

		if (sumatoria >= tamanio_seg)																	//porque los segmentos no estan compactados (hay espacio pero disperso).
		{
			log_info(logger, "Solicitud de Compactacion");
			// INFORMAR KERNEL COMPACTAR !!!
		}

		else																							//porque no hay mas espacio (si se compactan los segmentos igual no hay espacio).
		{
			log_info(logger, "No hay mas memoria");
			// FALTA DE ESPACIO LIBRE KERNEL !!!
		}
	}
}

//Busca el segmento por su id y lo elimina
void eliminar_segmento(unsigned int pid, uint32_t id)
{
	segmento *seg;
	segmento *hueco;
	int ady;

	for (int i = 0; i < list_size(tabla_segmentos_total); i++)
	{
		seg = list_get(tabla_segmentos_total, i);
		if (seg->id == id)																				//Encuentra el segmento.
		{
			log_info(logger, "PID: %u - Eliminar Segmento: %d - Base: %d - Tamanio: %d", pid, seg->id, seg->direccion_base, seg->tam_segmento);
			list_remove(tabla_segmentos_total, i);														//Lo borra de la tabla de segmentos.
			ady = agrupar_huecos(seg->direccion_base, seg->direccion_limite);							//Si tiene huecos aledanios, los agrupa.
			if (ady == 0)																				//Si no los tiene,
			{
				hueco = malloc(sizeof(segmento));
				hueco->direccion_base = seg->direccion_base;
				hueco->direccion_limite = seg->direccion_limite;
				list_add(huecos, hueco);																//crea el hueco.
			}
			free(seg);

			break;
		}
	}
}

//Si tiene huecos aledanios los agrupa y devuelve 1, sino devuelve 0
int agrupar_huecos(uint32_t base, uint32_t limite)
{
	segmento *hueco_izquierdo = NULL;
	int index_izq;
	segmento *hueco_derecho = NULL;
	int index_der;
	segmento *hueco;
	segmento *hueco_agrupado;

	for (int i = 0; i < list_size(huecos); i++)
	{
		hueco = list_get(huecos, i);
		if ((hueco->direccion_limite + 1) == base)
		{
			hueco_izquierdo = hueco;
			index_izq = i;
			break;
		}
	}

	for (int j = 0; j < list_size(huecos); j++)
	{
		hueco = list_get(huecos, j);
		if ((hueco->direccion_base - 1) == limite)
		{
			hueco_derecho = hueco;
			index_der = j;
			break;
		}
	}

	if (hueco_izquierdo != NULL && hueco_derecho != NULL)
	{

		list_remove(huecos, index_izq);
		list_remove(huecos, index_der);

		hueco_agrupado = malloc(sizeof(segmento));
		hueco_agrupado->direccion_base = hueco_derecho->direccion_limite;
		hueco_agrupado->direccion_limite = hueco_izquierdo->direccion_limite;
		free(hueco_derecho);
		free(hueco_izquierdo);

		list_add(huecos, hueco_agrupado);

		return 1;
	}

	else
	{
		return 0;
	}
}
/*
int list_index(t_list* lista, void* element) {
    int index = 0;
    t_link_element* actual = lista->head;

    while (actual != NULL) {
        if (actual->data == element) {
            return index;
        }
        actual = actual->next;
        index++;
    }

    return -1;  // Element not found
}


int comparar_segmentos(void *seg1, void *seg2)
{
	segmento *segmento1 = (segmento *)seg1;
	segmento *segmento2 = (segmento *)seg2;

	if (segmento1->direccion_base < segmento2->direccion_base)
	{
		return -1;
	}
	else if (segmento1->direccion_base > segmento2->direccion_base)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void obtener_huecos_libres ()
{
	list_sort(tabla_segmentos_total, comparar_segmentos);

	segmento* segmento_actual;
	segmento* hueco = (segmento*)(segmento*)malloc(sizeof(segmento));

	int direccion_base_anterior = 0;
	int direccion_limite_anterior = -1;

	for (int i = 0; i < list_size(tabla_segmentos_total); i++) {
			segmento_actual = list_get(tabla_segmentos_total, i);

			if (direccion_base_anterior < segmento_actual->direccion_base) // Hay un hueco libre entre los segmentos anteriores y el actual
			{
				hueco -> pid = -2;
				hueco->direccion_base = direccion_limite_anterior + 1;
				hueco->direccion_limite = segmento_actual->direccion_base - 1;
				hueco->tam_segmento = (segmento_actual->direccion_base - 1) - (direccion_limite_anterior + 1);

				list_add (huecos,hueco);
			}

			direccion_base_anterior = segmento_actual->direccion_base;
			direccion_limite_anterior = segmento_actual->direccion_limite;
	}

}*/

//Suma los espacios de los huecos
uint32_t sumatoria_huecos()
{
	uint32_t sumatoria = 0;
	segmento *seg;

	for (int i = 0; i < list_size(huecos); i++)
	{
		seg = (segmento *)list_get(huecos, i);
		sumatoria += seg->tam_segmento;
	}

	return sumatoria;
}

//Si hay espacio disponible devuelve 1, sino devuelve 0
int hay_espacio_disponible(int tam_segmento)
{
	int espacio_disponible;
	segmento *segmento_actual;
	segmento *segmento_siguiente;

	for (int i = 0; i < list_size(memoria_usuario); i++)
	{																									// recorre toda la memoria
		segmento_actual = list_get(memoria_usuario, i); 												// toma un segmento

		if (i + 1 < list_size(memoria_usuario))
		{																										 // si no es el ultimo
			segmento_siguiente = list_get(memoria_usuario, i + 1);												 // toma el siguiente
			espacio_disponible = (segmento_siguiente->direccion_base) - (segmento_actual->direccion_limite + 1); // a la direccion base de ese segmento le resta la direccion limite del seg actual + 1, se le suma uno pq seria donde supuestamente empieza el otro segmento

			if (espacio_disponible >= tam_segmento)
			{ // despues de calcular la resta, se fija si entra el segmento en ese lugar (si hay espacio en la memoria)
				return 1;
			}
		}
	}
	return 0;
}


void eliminar_hueco(uint32_t base, uint32_t limite)
{
	segmento* hueco;

	for (int i = 0; i < list_size(huecos); i++)
		{
			hueco = list_get(huecos, i);
			if (hueco->direccion_limite == base && hueco->direccion_limite == limite)
			{
				list_remove(huecos, i);
				break;
			}
		}
}


// Busca el primer hueco disponible desde el comienzo de memoria
void first_fit(unsigned int pid_proceso, uint32_t tam_segmento)
{
	int segmento_asignado = -1;
	segmento *segmento_actual;
	segmento *segmento_siguiente;
	uint32_t espacio_libre;
	segmento *nuevo_segmento;

	for (int i = 0; i < list_size(memoria_usuario); i++)
	{

		segmento_actual = list_get(memoria_usuario, i); 													// Recorre todos los segmentos

		if (i + 1 < config_mem.tam_memoria)																	// Ve si existe otro segmento
		{
			segmento_siguiente = list_get(memoria_usuario, i + 1);

			espacio_libre = segmento_siguiente->direccion_base - (segmento_actual->direccion_limite + 1); 	// Si existe, calcula el espacio entre ambos

			if (espacio_libre >= tam_segmento)																// Si entra el segmento en el espacio, lo agrega
			{
				segmento_asignado = segmento_actual->id + 1; // ID del nuevo segmento
				nuevo_segmento = (segmento *)malloc(sizeof(segmento));
				nuevo_segmento->pid = pid_proceso;
				nuevo_segmento->id = segmento_asignado;
				nuevo_segmento->tam_segmento = tam_segmento;
				nuevo_segmento->direccion_base = segmento_actual->direccion_limite + 1;
				nuevo_segmento->direccion_limite = nuevo_segmento->direccion_base + tam_segmento - 1;
				list_add_in_index(memoria_usuario, i + 1, nuevo_segmento);
				log_info(logger, "PID: %u - Crear Segmento: %d - Base: %d - Tamanio: %d", pid_proceso, segmento_asignado, nuevo_segmento->direccion_base, tam_segmento);
				eliminar_hueco(nuevo_segmento->direccion_base, nuevo_segmento->direccion_limite);

				break;
			}
		}
	}
}

// Busca el hueco mas chico donde entre el proceso
void best_fit(unsigned int pid_proceso, uint32_t tam_segmento)
{
	int segmento_asignado = -1;
	int mejor_ajuste = INT_MAX;
	segmento *segmento_actual;
	segmento *segmento_siguiente;
	int espacio_libre;
	segmento *nuevo_segmento;
	uint32_t nueva_dir_base;

	for (int i = 0; i < list_size(memoria_usuario); i++)
	{
		segmento_actual = list_get(memoria_usuario, i);

		if (i + 1 < list_size(memoria_usuario))
		{
			segmento_siguiente = list_get(memoria_usuario, i + 1);
			espacio_libre = segmento_siguiente->direccion_base - (segmento_actual->direccion_limite + 1);

			if (espacio_libre >= tam_segmento && espacio_libre < mejor_ajuste)								//Si entra el segmento y si el espacio es menor al menor espacio, lo agrega.
			{
				segmento_asignado = segmento_actual->id + 1;
				mejor_ajuste = espacio_libre;
				nueva_dir_base = segmento_actual->direccion_limite + 1;
			}
		}
	}

	if (segmento_asignado != -1)
	{
		nuevo_segmento = (segmento *)malloc(sizeof(segmento));
		nuevo_segmento->id = segmento_asignado;
		nuevo_segmento->tam_segmento = tam_segmento;
		nuevo_segmento->direccion_base = nueva_dir_base;
		nuevo_segmento->direccion_limite = nuevo_segmento->direccion_base + tam_segmento - 1;

		// Insertar el nuevo segmento en la lista de memoria después del segmento anterior al segmento asignado
		list_add_in_index(memoria_usuario, segmento_asignado, nuevo_segmento);
		log_info(logger, "PID: %u - Crear Segmento: %d - Base: %d - Tamanio: %d", pid_proceso, segmento_asignado, nuevo_segmento->direccion_base, tam_segmento);
		eliminar_hueco(nuevo_segmento->direccion_base, nuevo_segmento->direccion_limite);
	}
}

// Buscar el hueco más grande que pueda contener el nuevo segmento
void worst_fit(unsigned int pid_proceso, uint32_t tam_segmento)
{
	int segmento_asignado = -1;
	int mejor_ajuste = INT_MIN;
	segmento *segmento_actual;
	int espacio_libre;
	segmento *segmento_siguiente;
	uint32_t nueva_dir_base;


	for (int i = 0; i < list_size(memoria_usuario); i++)
	{
		segmento_actual = list_get(memoria_usuario, i);

		if (i + 1 < list_size(memoria_usuario))
		{
			segmento_siguiente = list_get(memoria_usuario, i + 1);
			espacio_libre = segmento_siguiente->direccion_base - (segmento_actual->direccion_limite + 1);

			if (espacio_libre >= tam_segmento && espacio_libre > mejor_ajuste)								//Si entra el segmento y si el espacio es mayor al mayor espacio, lo agrega.
			{
				segmento_asignado = segmento_actual->id + 1;
				mejor_ajuste = espacio_libre;
				nueva_dir_base = (segmento_actual->direccion_limite) + 1;
			}
		}
	}

	if (segmento_asignado != -1)
	{
		// Crear el nuevo segmento y establecer sus direcciones
		segmento *nuevo_segmento = (segmento *)malloc(sizeof(segmento));
		nuevo_segmento->id = segmento_asignado;
		nuevo_segmento->tam_segmento = tam_segmento;
		nuevo_segmento->direccion_base = nueva_dir_base; // list_get(memoria_usuario, segmento_asignado - 1)->direccion_limite + 1;
		nuevo_segmento->direccion_limite = nuevo_segmento->direccion_base + tam_segmento - 1;

		// Insertar el nuevo segmento en la lista de memoria después del segmento anterior al segmento asignado
		list_add_in_index(memoria_usuario, segmento_asignado, nuevo_segmento);
		log_info(logger, "PID: %u - Crear Segmento: %d - Base: %d - Tamanio: %d", pid_proceso, segmento_asignado, nuevo_segmento->direccion_base, tam_segmento);
		eliminar_hueco(nuevo_segmento->direccion_base, nuevo_segmento->direccion_limite);
	}
}

//                                    ACCESO A ESPACIO USUARIO - COMENTADO PQ NO SE COMO LLEGA LA INFO

/*
void manejo_instrucciones (inst_mem tipo_instruccion, uint32_t dir_dada, char* origen, uint32_t nuevo_valor,t_list* instrucciones, int* socket_cliente){
	//inst_mem tipo_instruccion = *(inst_mem *) list_get (instrucciones,0);
	//uint32_t dir_dada = *(uint32_t*) list_get(instrucciones,1);                                                             //Direccion fisica dada por CPU o FS


	uint32_t valor;
//	uint32_t nuevo_valor;
	//int boolEsc;


		switch (tipo_instruccion){
		case M_READ:                                                                                 //Ante un pedido de lectura, devolver el valor que se encuentra en la posicion pedida.
			delay (config_mem.retardo_memoria);                                            //Tiempo de espera
				//Microsegundos = Milisegundos * 1000
			valor = leer_memoria (dir_dada);                                               		   //Busca y retorna el valor en la direccion de memoria dada
			log_info (logger, "Se leyo el valor %d en la posicion %d", valor, dir_dada);


			//AGREGAR TD LO DE PAQUETES !!!

			break;

		case M_WRITE:                                                                                // Ante un pedido de escritura, escribir lo indicado en la posición pedida y responder un mensaje de ‘OK’.
			//nuevo_valor = *(uint32_t*) list_get(instrucciones,2);

			delay (config_mem.retardo_memoria);
			escribir_memoria (nuevo_valor, dir_dada);
			//responder con un msj OK !!!
			log_info (logger, "Se escribio el valor %d en la posicion %d", nuevo_valor, dir_dada);
			break;

		default:
					log_warning(logger,"Operacion desconocida \n");
					break;
		}

}*/

void* leer_memoria(uint32_t id_buscado, uint32_t desp, size_t tamanio)
{
	segmento *seg;
	void* valorLeido;
	int direccion;

	for (int i = 0; i < list_size(tabla_segmentos_total); i++)
	{
		seg = list_get(tabla_segmentos_total, i);

		if (seg->id == id_buscado)
		{
			direccion = seg->direccion_base + desp;
			valorLeido = malloc(tamanio);
			delay (config_mem.retardo_memoria);
			memcpy(valorLeido, memoria_usuario + direccion, tamanio);
			return valorLeido;
		}
	}

	return 0;


void escribir_memoria(uint32_t id_buscado, uint32_t desp, void* nuevo_valor, size_t tamanio)
{
	segmento *seg;
	int direccion;

	for (int i = 0; i < list_size(tabla_segmentos_total); i++)
		{
			seg = list_get(tabla_segmentos_total, i);

			if (seg->id == id_buscado)
			{
				direccion = seg->direccion_base + desp;
				delay (config_mem.retardo_memoria);
				memcpy(memoria_usuario + direccion, nuevo_valor, tamanio);
				return;
		}
	}
}


void terminar_memoria(t_log *logger, t_config *config, int socket)
{
	list_destroy_and_destroy_elements(tabla_segmentos_total, free);
	list_destroy_and_destroy_elements(huecos, free);
	free(memoria_usuario);

	log_info(logger, "Finalizando memoria");
	if (logger != NULL)
	{
		log_destroy(logger);
	}
	if (config != NULL)
	{
		config_destroy(config);
	}

	liberar_servidor(&socket);
}
