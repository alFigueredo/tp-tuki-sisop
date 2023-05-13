#include "proceso.h"

pcb* recibir_pcb(t_list* lista) {
	pcb* proceso = malloc(sizeof(pcb));
	memcpy(&(proceso->pid), list_remove(lista, 0), sizeof(unsigned int));
	int instrucciones_size;
	memcpy(&(instrucciones_size), list_remove(lista, 0), sizeof(int));
	proceso->instrucciones = list_take_and_remove(lista, instrucciones_size);
	memcpy(&(proceso->program_counter), list_remove(lista, 0), sizeof(int));
	memcpy(proceso->registros.AX, list_remove(lista, 0), 4);
	memcpy(proceso->registros.BX, list_remove(lista, 0), 4);
	memcpy(proceso->registros.CX, list_remove(lista, 0), 4);
	memcpy(proceso->registros.DX, list_remove(lista, 0), 4);
	memcpy(proceso->registros.EAX, list_remove(lista, 0), 8);
	memcpy(proceso->registros.EBX, list_remove(lista, 0), 8);
	memcpy(proceso->registros.ECX, list_remove(lista, 0), 8);
	memcpy(proceso->registros.EDX, list_remove(lista, 0), 8);
	memcpy(proceso->registros.RAX, list_remove(lista, 0), 16);
	memcpy(proceso->registros.RBX, list_remove(lista, 0), 16);
	memcpy(proceso->registros.RCX, list_remove(lista, 0), 16);
	memcpy(proceso->registros.RDX, list_remove(lista, 0), 16);
	return proceso;
}

void enviar_pcb(int conexion, pcb* proceso, op_code estado) {
	t_paquete* paquete = crear_paquete(estado);
	agregar_a_paquete(paquete, &(proceso->pid), sizeof(unsigned int));
	int instrucciones_size = list_size(proceso->instrucciones);
	agregar_a_paquete(paquete, &instrucciones_size, sizeof(int));
	for (int i=0; i<instrucciones_size; i++) {
		char* instruccion = list_get(proceso->instrucciones, i);
		agregar_a_paquete(paquete, instruccion, strlen(instruccion)+1);
	}
	agregar_a_paquete(paquete, &(proceso->program_counter), sizeof(int));
	agregar_a_paquete(paquete, proceso->registros.AX, 4);
	agregar_a_paquete(paquete, proceso->registros.BX, 4);
	agregar_a_paquete(paquete, proceso->registros.CX, 4);
	agregar_a_paquete(paquete, proceso->registros.DX, 4);
	agregar_a_paquete(paquete, proceso->registros.EAX, 8);
	agregar_a_paquete(paquete, proceso->registros.EBX, 8);
	agregar_a_paquete(paquete, proceso->registros.ECX, 8);
	agregar_a_paquete(paquete, proceso->registros.EDX, 8);
	agregar_a_paquete(paquete, proceso->registros.RAX, 16);
	agregar_a_paquete(paquete, proceso->registros.RBX, 16);
	agregar_a_paquete(paquete, proceso->registros.RCX, 16);
	agregar_a_paquete(paquete, proceso->registros.RDX, 16);
	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
}

void delay(int milliseconds)
{
	t_temporal* clock = temporal_create();
	while(temporal_gettime(clock)<milliseconds)
		;
	temporal_destroy(clock);
}

t_dictionary* diccionario_instrucciones(void) {
	t_dictionary* instrucciones = dictionary_create();
	dictionary_put(instrucciones, "SET", (void*)(intptr_t)SET);
	dictionary_put(instrucciones, "YIELD", (void*)(intptr_t)YIELD);
	// EXIT ya existe
	dictionary_put(instrucciones, "EXIT", (void*)(intptr_t)IEXIT);
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

int instruccion_set(t_dictionary* registros, char** parsed, pcb* proceso) {
	if (!dictionary_has_key(registros, parsed[1]))
		return 1;
	log_info(logger, "PID: %d - Ejecutando: %s - %s %s", proceso->pid, parsed[0], parsed[1], parsed[2]);
	memcpy(dictionary_get(registros, parsed[1]), parsed[2], strlen(parsed[2]));
	delay(config_get_int_value(config, "RETARDO_INSTRUCCION"));
	proceso->program_counter++;
	return 0;
}

void instruccion_yield(char** parsed, pcb* proceso) {
	log_info(logger, "PID: %d - Ejecutando: %s", proceso->pid, parsed[0]);;
	//log_debug(logger, "Registro AX: %s", string_substring_until(proceso->registros.AX, 4));
	//log_debug(logger, "Registro ECX: %s", string_substring_until(proceso->registros.ECX, 8));
	//log_debug(logger, "Registro BX: %s", string_substring_until(proceso->registros.BX, 4));
	proceso->program_counter++;
}

void instruccion_exit(char** parsed, pcb* proceso) {
	log_info(logger, "PID: %d - Ejecutando: %s", proceso->pid, parsed[0]);
	log_trace(logger, "Registro AX: %s", string_substring_until(proceso->registros.AX, 4));
	log_trace(logger, "Registro BX: %s", string_substring_until(proceso->registros.BX, 4));
	log_trace(logger, "Registro CX: %s", string_substring_until(proceso->registros.CX, 4));
	log_trace(logger, "Registro DX: %s", string_substring_until(proceso->registros.DX, 4));
	log_trace(logger, "Registro EAX: %s", string_substring_until(proceso->registros.EAX, 8));
	log_trace(logger, "Registro EBX: %s", string_substring_until(proceso->registros.EBX, 8));
	log_trace(logger, "Registro ECX: %s", string_substring_until(proceso->registros.ECX, 8));
	log_trace(logger, "Registro EDX: %s", string_substring_until(proceso->registros.EDX, 8));
	log_trace(logger, "Registro RAX: %s", string_substring_until(proceso->registros.RAX, 16));
	log_trace(logger, "Registro RBX: %s", string_substring_until(proceso->registros.RBX, 16));
	log_trace(logger, "Registro RCX: %s", string_substring_until(proceso->registros.RCX, 16));
	log_trace(logger, "Registro RDX: %s", string_substring_until(proceso->registros.RDX, 16));
	proceso->program_counter++;
}

enum_instrucciones interpretar_instrucciones(pcb* proceso) {
	t_dictionary* instrucciones = diccionario_instrucciones();
	t_dictionary* registros = diccionario_registros(&proceso->registros);
	while (proceso->program_counter<list_size(proceso->instrucciones)) {
		char* instruccion = list_get(proceso->instrucciones, proceso->program_counter);
		char** parsed = string_split(instruccion, " ");
		int instruccion_enum = -2;
		if (dictionary_has_key(instrucciones, parsed[0]))
			instruccion_enum = (int)(intptr_t)dictionary_get(instrucciones, parsed[0]);
		else
			instruccion_enum = -1;
		switch (instruccion_enum) {
			case SET:
				if (instruccion_set(registros, parsed, proceso)) {
					log_warning(logger, "PID: %d - Advertencia: No se pudo interpretar la instrucción - Ejecutando: EXIT", proceso->pid);
					destruir_diccionarios(instrucciones, registros);
					return EXIT;
				}
				break;
			case YIELD:
				instruccion_yield(parsed, proceso);
				destruir_diccionarios(instrucciones, registros);
				return READY;
			case IEXIT:
				instruccion_exit(parsed, proceso);
				destruir_diccionarios(instrucciones, registros);
				return EXIT;
			case -1:
				log_warning(logger, "PID: %d - Advertencia: No se pudo interpretar la instrucción - Ejecutando: EXIT", proceso->pid);
				destruir_diccionarios(instrucciones, registros);
				return EXIT;
		}
	}
	log_warning(logger, "PID: %d - Advertencia: Sin instrucciones por ejecutar - Ejecutando: EXIT", proceso->pid);
	destruir_diccionarios(instrucciones, registros);
	return EXIT;
}
