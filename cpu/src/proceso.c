#include "proceso.h"

t_dictionary* diccionario_instrucciones(void) {
	t_dictionary* instrucciones = dictionary_create();
	dictionary_put(instrucciones, "SET", (void*)(intptr_t)SET);
	dictionary_put(instrucciones, "MOV_IN", (void*)(intptr_t)MOV_IN);
	dictionary_put(instrucciones, "MOV_OUT", (void*)(intptr_t)MOV_OUT);
	dictionary_put(instrucciones, "I/O", (void*)(intptr_t)I_O);
	dictionary_put(instrucciones, "F_OPEN", (void*)(intptr_t)F_OPEN);
	dictionary_put(instrucciones, "F_CLOSE", (void*)(intptr_t)F_CLOSE);
	dictionary_put(instrucciones, "F_SEEK", (void*)(intptr_t)F_SEEK);
	dictionary_put(instrucciones, "F_READ", (void*)(intptr_t)F_READ);
	dictionary_put(instrucciones, "F_WRITE", (void*)(intptr_t)F_WRITE);
	dictionary_put(instrucciones, "F_TRUNCATE", (void*)(intptr_t)F_TRUNCATE);
	dictionary_put(instrucciones, "WAIT", (void*)(intptr_t)I_WAIT);
	dictionary_put(instrucciones, "SIGNAL", (void*)(intptr_t)I_SIGNAL);
	dictionary_put(instrucciones, "CREATE_SEGMENT", (void*)(intptr_t)CREATE_SEGMENT);
	dictionary_put(instrucciones, "DELETE_SEGMENT", (void*)(intptr_t)DELETE_SEGMENT);
	dictionary_put(instrucciones, "YIELD", (void*)(intptr_t)YIELD);
	dictionary_put(instrucciones, "EXIT", (void*)(intptr_t)I_EXIT);
	return instrucciones;
}

t_dictionary* diccionario_registros(registros_cpu* registro) {
	t_dictionary* registros = dictionary_create();
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
	return registros;
}

void destruir_diccionarios(t_dictionary* instrucciones, t_dictionary* registros) {
	dictionary_destroy(instrucciones);
	dictionary_destroy(registros);
}

enum_instrucciones interpretar_instrucciones(pcb* proceso) {
	t_dictionary* instrucciones = diccionario_instrucciones();
	t_dictionary* registros = diccionario_registros(&proceso->registros);
	while (proceso->program_counter<list_size(proceso->instrucciones)) {
		char* instruccion = list_get(proceso->instrucciones, proceso->program_counter);
		// log_debug(logger, "Instrucción: %s0", instruccion);
    	char** parsed = string_split(instruccion, " "); //Partes de la instruccion actual
		int instruccion_enum = (int)(intptr_t)dictionary_get(instrucciones, parsed[0]);
		switch (instruccion_enum) {
			case SET:
				instruccion_set(registros, parsed, proceso);
				break;
			case MOV_IN:
				instruccion_mov_in(registros, parsed, proceso);
				break;
			case MOV_OUT:
				instruccion_mov_out(registros, parsed, proceso);
				break;
			case I_O:
				instruccion_i_o(parsed, proceso);
				destruir_diccionarios(instrucciones, registros);
				string_array_destroy(parsed);
				return IO_BLOCK;
			case F_OPEN:
				instruccion_f_open(parsed, proceso);
				break;
			case F_CLOSE:
				instruccion_f_close(parsed, proceso);
				break;
			case F_SEEK:
				instruccion_f_seek(parsed, proceso);
				break;
			case F_READ:
				instruccion_f_read(parsed, proceso);
				break;
			case F_WRITE:
				instruccion_f_write(parsed, proceso);
				break;
			case F_TRUNCATE:
				instruccion_f_truncate(parsed, proceso);
				break;
			case I_WAIT:
				instruccion_wait(parsed, proceso);
				destruir_diccionarios(instrucciones, registros);
				string_array_destroy(parsed);
				return WAIT;
			case I_SIGNAL:
				instruccion_signal(parsed, proceso);
				destruir_diccionarios(instrucciones, registros);
				string_array_destroy(parsed);
				return SIGNAL;
			case CREATE_SEGMENT:
				instruccion_create_segment(parsed, proceso);
				break;
			case DELETE_SEGMENT:
				instruccion_delete_segment(parsed, proceso);
				break;
			case YIELD:
				instruccion_yield(parsed, proceso);
				destruir_diccionarios(instrucciones, registros);
				string_array_destroy(parsed);
				return READY;
			case I_EXIT:
				instruccion_exit(parsed, proceso);
				destruir_diccionarios(instrucciones, registros);
				string_array_destroy(parsed);
				return EXIT;
			case -1:
				log_warning(logger, "PID: %d - Advertencia: No se pudo interpretar la instrucción - Ejecutando: EXIT", proceso->pid);
				destruir_diccionarios(instrucciones, registros);
				string_array_destroy(parsed);
				return EXIT;
		}
		string_array_destroy(parsed);
	}
	log_warning(logger, "PID: %d - Advertencia: Sin instrucciones por ejecutar - Ejecutando: EXIT", proceso->pid);
	destruir_diccionarios(instrucciones, registros);
	return EXIT;
}

void instruccion_set(t_dictionary* registros, char** parsed, pcb* proceso) {
	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	memcpy(dictionary_get(registros, parsed[1]), parsed[2], strlen(parsed[2]));
	delay(config_get_int_value(config, "RETARDO_INSTRUCCION"));
	proceso->program_counter++;
}

void instruccion_mov_in(t_dictionary* registros, char** parsed, pcb* proceso)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	log_warning(logger, "PID: %d - Advertencia: Instruccion sin realizar", proceso->pid);
	proceso->program_counter++;
}

void instruccion_mov_out(t_dictionary* registros, char** parsed, pcb* proceso)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	log_warning(logger, "PID: %d - Advertencia: Instruccion sin realizar", proceso->pid);
	proceso->program_counter++;
}

void instruccion_i_o(char** parsed, pcb* proceso)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s", proceso->pid, parsed[0], parsed[1]);
	proceso->program_counter++;
}

void instruccion_f_open(char** parsed, pcb* proceso)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s", proceso->pid, parsed[0], parsed[1]);
	log_warning(logger, "PID: %d - Advertencia: Instruccion sin realizar", proceso->pid);
	proceso->program_counter++;
}

void instruccion_f_close(char** parsed, pcb* proceso)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s", proceso->pid, parsed[0], parsed[1]);
	log_warning(logger, "PID: %d - Advertencia: Instruccion sin realizar", proceso->pid);
	proceso->program_counter++;
}

void instruccion_f_seek(char** parsed, pcb* proceso)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	log_warning(logger, "PID: %d - Advertencia: Instruccion sin realizar", proceso->pid);
	proceso->program_counter++;
}

void instruccion_f_read(char** parsed, pcb* proceso)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	log_warning(logger, "PID: %d - Advertencia: Instruccion sin realizar", proceso->pid);
	proceso->program_counter++;
}

void instruccion_f_write(char** parsed, pcb* proceso)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s %s %s", proceso->pid, parsed[0], parsed[1], parsed[2], parsed[3]);
	log_warning(logger, "PID: %d - Advertencia: Instruccion sin realizar", proceso->pid);
	proceso->program_counter++;
}

void instruccion_f_truncate(char** parsed, pcb* proceso)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	log_warning(logger, "PID: %d - Advertencia: Instruccion sin realizar", proceso->pid);
	proceso->program_counter++;
}

void instruccion_wait(char** parsed, pcb* proceso)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s", proceso->pid, parsed[0], parsed[1]);
	proceso->program_counter++;
}

void instruccion_signal(char** parsed, pcb* proceso)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s", proceso->pid, parsed[0], parsed[1]);
	proceso->program_counter++;
}

void instruccion_create_segment(char** parsed, pcb* proceso)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	log_warning(logger, "PID: %d - Advertencia: Instruccion sin realizar", proceso->pid);
	proceso->program_counter++;
}

void instruccion_delete_segment(char** parsed, pcb* proceso)
{
	log_info(logger, "PID: %d - Ejecutando: %s - %s", proceso->pid, parsed[0], parsed[1]);
	log_warning(logger, "PID: %d - Advertencia: Instruccion sin realizar", proceso->pid);
	proceso->program_counter++;
}

void instruccion_yield(char** parsed, pcb* proceso) {
	log_info(logger, "PID: %d - Ejecutando: %s", proceso->pid, parsed[0]);
	proceso->program_counter++;
}

void instruccion_exit(char** parsed, pcb* proceso) {
	log_info(logger, "PID: %d - Ejecutando: %s", proceso->pid, parsed[0]);
	proceso->program_counter++;
}
