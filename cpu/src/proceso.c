#include "proceso.h"

int conexion_kernel;
int conexion_memoria;
pcb* proceso;
t_dictionary* instrucciones;
t_dictionary* registros;


void iniciar_diccionario_instrucciones(void) {
	instrucciones = dictionary_create();
	dictionary_put(instrucciones, "SET", (void*)(intptr_t)I_SET);
	dictionary_put(instrucciones, "MOV_IN", (void*)(intptr_t)I_MOV_IN);
	dictionary_put(instrucciones, "MOV_OUT", (void*)(intptr_t)I_MOV_OUT);
	dictionary_put(instrucciones, "I/O", (void*)(intptr_t)I_IO);
	dictionary_put(instrucciones, "F_OPEN", (void*)(intptr_t)I_F_OPEN);
	dictionary_put(instrucciones, "F_CLOSE", (void*)(intptr_t)I_F_CLOSE);
	dictionary_put(instrucciones, "F_SEEK", (void*)(intptr_t)I_F_SEEK);
	dictionary_put(instrucciones, "F_READ", (void*)(intptr_t)I_F_READ);
	dictionary_put(instrucciones, "F_WRITE", (void*)(intptr_t)I_F_WRITE);
	dictionary_put(instrucciones, "F_TRUNCATE", (void*)(intptr_t)I_F_TRUNCATE);
	dictionary_put(instrucciones, "WAIT", (void*)(intptr_t)I_WAIT);
	dictionary_put(instrucciones, "SIGNAL", (void*)(intptr_t)I_SIGNAL);
	dictionary_put(instrucciones, "CREATE_SEGMENT", (void*)(intptr_t)I_CREATE_SEGMENT);
	dictionary_put(instrucciones, "DELETE_SEGMENT", (void*)(intptr_t)I_DELETE_SEGMENT);
	dictionary_put(instrucciones, "YIELD", (void*)(intptr_t)I_YIELD);
	dictionary_put(instrucciones, "EXIT", (void*)(intptr_t)I_EXIT);
}

void iniciar_diccionario_registros(registros_cpu* registro) {
	registros = dictionary_create();
	dictionary_put(registros, "AX", (void*)registro->AX);
	dictionary_put(registros, "BX", (void*)registro->BX);
	dictionary_put(registros, "CX", (void*)registro->CX);
	dictionary_put(registros, "DX", (void*)registro->DX);
	dictionary_put(registros, "EAX", (void*)registro->EAX);
	dictionary_put(registros, "EBX", (void*)registro->EBX);
	dictionary_put(registros, "ECX", (void*)registro->ECX);
	dictionary_put(registros, "EDX", (void*)registro->EDX);
	dictionary_put(registros, "RAX", (void*)registro->RAX);
	dictionary_put(registros, "RBX", (void*)registro->RBX);
	dictionary_put(registros, "RCX", (void*)registro->RCX);
	dictionary_put(registros, "RDX", (void*)registro->RDX);
}

void destruir_diccionarios(void) {
	dictionary_destroy(instrucciones);
	dictionary_destroy(registros);
}

void interpretar_instrucciones(void) {
	iniciar_diccionario_instrucciones();
	iniciar_diccionario_registros(&proceso->registros);
	while (proceso->program_counter<list_size(proceso->instrucciones)) {
		// char* instruccion = list_get(proceso->instrucciones, proceso->program_counter);
		// log_debug(logger, "Instrucción: %s0", instruccion);
    	char** parsed = string_split((char*)list_get(proceso->instrucciones, proceso->program_counter), " "); //Partes de la instruccion actual
		int instruccion_enum = (int)(intptr_t)dictionary_get(instrucciones, parsed[0]);
		switch (instruccion_enum) {
			case I_SET:
				instruccion_set(parsed);
				break;
			case I_MOV_IN:
				instruccion_mov_in(parsed);
				return;
				// break;
			case I_MOV_OUT:
				instruccion_mov_out(parsed);
				return;
				// break;
			case I_IO:
				instruccion_i_o(parsed);
				destruir_diccionarios();
				return;
			case I_F_OPEN:
				instruccion_f_open(parsed);
				return;
				// break;
			case I_F_CLOSE:
				instruccion_f_close(parsed);
				return;
				// break;
			case I_F_SEEK:
				instruccion_f_seek(parsed);
				return;
				// break;
			case I_F_READ:
				instruccion_f_read(parsed);
				// return;
				break;
			case I_F_WRITE:
				instruccion_f_write(parsed);
				// return;
				break;
			case I_F_TRUNCATE:
				instruccion_f_truncate(parsed);
				return;
				// break;
			case I_WAIT:
				instruccion_wait(parsed);
				destruir_diccionarios();
				return;
			case I_SIGNAL:
				instruccion_signal(parsed);
				destruir_diccionarios();
				return;
			case I_CREATE_SEGMENT:
				instruccion_create_segment(parsed);
				return;
				// break;
			case I_DELETE_SEGMENT:
				instruccion_delete_segment(parsed);
				return;
				// break;
			case I_YIELD:
				instruccion_yield(parsed);
				destruir_diccionarios();
				return;
			case I_EXIT:
				instruccion_exit(parsed);
				destruir_diccionarios();
				return;
			case -1:
				log_warning(logger, "PID: %d - Advertencia: No se pudo interpretar la instrucción - Ejecutando: EXIT", proceso->pid);
				error_exit(EXIT);
				destruir_diccionarios();
				return;
		}
		string_array_destroy(parsed);
	}
	log_warning(logger, "PID: %d - Advertencia: Sin instrucciones por ejecutar - Ejecutando: EXIT", proceso->pid);
	error_exit(EXIT);
	destruir_diccionarios();
	return;
}

void instruccion_set(char** parsed) {
	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	memcpy(dictionary_get(registros, parsed[1]), parsed[2], strlen(parsed[2]));
	delay(config_get_int_value(config, "RETARDO_INSTRUCCION"));
	proceso->program_counter++;
}

void instruccion_mov_in(char** parsed)
{
	// Para probar la función hay que descomentar lo comentado, descomentar el return en el case correspondiente, y comentar en donde se modifica el program_counter

	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	char* dir_fisica = traducir_dir_logica(parsed[2]);
	if (strcmp(dir_fisica, "SEG_FAULT")==0) {
		log_error(logger, "Error: segmentation fault");
		error_exit(EXIT_SEG_FAULT);
		return;
	}
	// int tamanio_dato = parsed[1][0]=='R' ? 16 : parsed[1][0]=='E' ? 8 : 4;
	char* instruccion = string_from_format("%s %s %s", parsed[0], parsed[1], dir_fisica);
	char* ins_alloc = malloc(strlen(instruccion)+1);
	strcpy(ins_alloc, instruccion);
	list_replace_and_destroy_element(proceso->instrucciones, proceso->program_counter, ins_alloc, free);
	log_trace(logger, "PID: %d - Instruccion traducida: %s", proceso->pid, (char*)list_get(proceso->instrucciones, proceso->program_counter));
	t_instruccion* instruccion_proceso = malloc(sizeof(t_instruccion));
	generar_instruccion(proceso, instruccion_proceso, (char*)list_get(proceso->instrucciones, proceso->program_counter));
	enviar_instruccion(conexion_memoria, instruccion_proceso, MOV_IN);
	// proceso->program_counter++;
}


void mov_in(t_instruccion* instruccion_proceso) {
	char** parsed = string_split(instruccion_proceso->instruccion, " ");
	memcpy(dictionary_get(registros, parsed[1]), instruccion_proceso->dato, instruccion_proceso->tamanio_dato);
	string_array_destroy(parsed);
	proceso->program_counter++;
}

void instruccion_mov_out(char** parsed)
{
	// Para probar la función hay que descomentar lo comentado, descomentar el return en el case correspondiente, y comentar en donde se modifica el program_counter

	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	char* dir_fisica = traducir_dir_logica(parsed[1]);
	if (strcmp(dir_fisica, "SEG_FAULT")==0) {
		log_error(logger, "Error: segmentation fault");
		error_exit(EXIT_SEG_FAULT);
		return;
	}
	int tamanio_dato = parsed[1][0]=='R' ? 16 : parsed[1][0]=='E' ? 8 : 4;
	char* instruccion = string_from_format("%s %s %s", parsed[0], dir_fisica, parsed[2]);
	char* ins_alloc = malloc(strlen(instruccion)+1);
	strcpy(ins_alloc, instruccion);
	list_replace_and_destroy_element(proceso->instrucciones, proceso->program_counter, ins_alloc, free);
	log_trace(logger, "PID: %d - Instruccion traducida: %s", proceso->pid, (char*)list_get(proceso->instrucciones, proceso->program_counter));
	t_instruccion* instruccion_proceso = malloc(sizeof(t_instruccion));
	generar_instruccion(proceso, instruccion_proceso, (char*)list_get(proceso->instrucciones, proceso->program_counter));
	instruccion_proceso->tamanio_dato = tamanio_dato;
	instruccion_proceso->dato = dictionary_get(registros, parsed[2]);
	enviar_instruccion_con_dato(conexion_memoria, instruccion_proceso, MOV_OUT);
	// proceso->program_counter++;
}

void instruccion_i_o(char** parsed)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s", proceso->pid, parsed[0], parsed[1]);
	proceso->program_counter++;
	enviar_pcb(conexion_kernel, proceso, IO_BLOCK);
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void instruccion_f_open(char** parsed)
{

	log_info(logger, "PID: %d - Ejecutando: %s - %s", proceso->pid, parsed[0], parsed[1]);
	proceso->program_counter++;
	enviar_pcb(conexion_kernel, proceso, F_OPEN);
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void instruccion_f_close(char** parsed)
{

	log_info(logger, "PID: %d - Ejecutando: %s - %s", proceso->pid, parsed[0], parsed[1]);
	proceso->program_counter++;
	enviar_pcb(conexion_kernel, proceso, F_CLOSE);
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void instruccion_f_seek(char** parsed)
{
	// Para probar la función hay que descomentar lo comentado y descomentar el return en el case correspondiente

	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	proceso->program_counter++;
	enviar_pcb(conexion_kernel, proceso, F_SEEK);
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void instruccion_f_read(char** parsed)
{
	// Para probar la función hay que descomentar lo comentado y descomentar el return en el case correspondiente

	log_info(logger, "PID: %d - Ejecutando: %s - %s %s %s", proceso->pid, parsed[0], parsed[1], parsed[2], parsed[3]);
	char* dir_fisica = traducir_dir_logica(parsed[2]);
	if (strcmp(dir_fisica, "SEG_FAULT")==0) {
		log_error(logger, "Error: segmentation fault");
		error_exit(EXIT_SEG_FAULT);
		return;
	}
	char* instruccion = string_from_format("%s %s %s %s", parsed[0], parsed[1], dir_fisica, parsed[3]);
	char* ins_alloc = malloc(strlen(instruccion)+1);
	strcpy(ins_alloc, instruccion);
	list_replace_and_destroy_element(proceso->instrucciones, proceso->program_counter, ins_alloc, free);
	log_trace(logger, "PID: %d - Instruccion traducida: %s", proceso->pid, (char*)list_get(proceso->instrucciones, proceso->program_counter));
	log_warning(logger, "PID: %d - Advertencia: Instruccion sin realizar", proceso->pid);
	proceso->program_counter++;
	// enviar_pcb(conexion_kernel, proceso, F_READ);
	// list_destroy_and_destroy_elements(proceso->instrucciones, free);
	// free(proceso);
}

void instruccion_f_write(char** parsed)
{
	// Para probar la función hay que descomentar lo comentado y descomentar el return en el case correspondiente

	log_info(logger, "PID: %d - Ejecutando: %s - %s %s %s", proceso->pid, parsed[0], parsed[1], parsed[2], parsed[3]);
	char* dir_fisica = traducir_dir_logica(parsed[2]);
	if (strcmp(dir_fisica, "SEG_FAULT")==0) {
		log_error(logger, "Error: segmentation fault");
		error_exit(EXIT_SEG_FAULT);
		return;
	}
	char* instruccion = string_from_format("%s %s %s %s", parsed[0], parsed[1], dir_fisica, parsed[3]);
	char* ins_alloc = malloc(strlen(instruccion)+1);
	strcpy(ins_alloc, instruccion);
	list_replace_and_destroy_element(proceso->instrucciones, proceso->program_counter, ins_alloc, free);
	log_trace(logger, "PID: %d - Instruccion traducida: %s", proceso->pid, (char*)list_get(proceso->instrucciones, proceso->program_counter));
	log_warning(logger, "PID: %d - Advertencia: Instruccion sin realizar", proceso->pid);
	proceso->program_counter++;
	// enviar_pcb(conexion_kernel, proceso, F_WRITE);
	// list_destroy_and_destroy_elements(proceso->instrucciones, free);
	// free(proceso);
}

void instruccion_f_truncate(char** parsed)
{
	// Para probar la función hay que descomentar lo comentado y descomentar el return en el case correspondiente

	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	proceso->program_counter++;
	enviar_pcb(conexion_kernel, proceso, F_TRUNCATE);
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void instruccion_wait(char** parsed)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s", proceso->pid, parsed[0], parsed[1]);
	proceso->program_counter++;
	enviar_pcb(conexion_kernel, proceso, WAIT);
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void instruccion_signal(char** parsed)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s", proceso->pid, parsed[0], parsed[1]);
	proceso->program_counter++;
	enviar_pcb(conexion_kernel, proceso, SIGNAL);
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void instruccion_create_segment(char** parsed)
{
	// Para probar la función hay que descomentar lo comentado y descomentar el return en el case correspondiente

	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	proceso->program_counter++;
	enviar_pcb(conexion_kernel, proceso, CREATE_SEGMENT);
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void instruccion_delete_segment(char** parsed)
{
	// Para probar la función hay que descomentar lo comentado y descomentar el return en el case correspondiente

	log_info(logger, "PID: %d - Ejecutando: %s - %s", proceso->pid, parsed[0], parsed[1]);
	proceso->program_counter++;
	enviar_pcb(conexion_kernel, proceso, DELETE_SEGMENT);
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void instruccion_yield(char** parsed) {
	log_info(logger, "PID: %d - Ejecutando: %s", proceso->pid, parsed[0]);
	proceso->program_counter++;
	enviar_pcb(conexion_kernel, proceso, READY);
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void instruccion_exit(char** parsed) {
	log_info(logger, "PID: %d - Ejecutando: %s", proceso->pid, parsed[0]);
	proceso->program_counter++;
	enviar_pcb(conexion_kernel, proceso, EXIT);
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void error_exit(op_code codigo) {
	enviar_pcb(conexion_kernel, proceso, codigo);
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

char* traducir_dir_logica(char* direccion_logica) {
	int tam_max_segmento = config_get_int_value(config, "TAM_MAX_SEGMENTO");
	int num_segmento = floor((double)atoi(direccion_logica)/tam_max_segmento);
	int desplazamiento_segmento = atoi(direccion_logica)%tam_max_segmento;
	// int segmento = obtener_segmento(num_segmento);
	int base_segmento;
	int tamanio_segmento;
	log_debug(logger, "Numero de segmento: %d - Desplazamiento: %d", num_segmento, desplazamiento_segmento);
	for (int i=0; i<list_size(proceso->tabla_segmentos); i++) {
		t_segmento* segmento_actual = list_get(proceso->tabla_segmentos, i);
		if (segmento_actual->id_segmento==num_segmento) {
			base_segmento = segmento_actual->direccion_base;
			tamanio_segmento = segmento_actual->tam_segmento;
			break;
		}
	}
	log_debug(logger, "Base: %d - Tamanio: %d", base_segmento, tamanio_segmento);
	if (desplazamiento_segmento>=tamanio_segmento)
		return "SEG_FAULT";
	int dir_fisica = base_segmento+desplazamiento_segmento;
	return string_from_format("%d", dir_fisica);
}
