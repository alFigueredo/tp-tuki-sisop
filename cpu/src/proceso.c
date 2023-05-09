#include "proceso.h"

pcb* recibir_pcb_de_kernel(t_list* lista) {
	pcb* proceso = malloc(sizeof(pcb));
	memcpy(&(proceso->pid), list_remove(lista, 0), sizeof(unsigned int));
	proceso->instrucciones = list_take_and_remove(lista, list_size(lista)-13);
	memcpy(&(proceso->program_counter), list_remove(lista, 0), sizeof(int));
	/*
	t_dictionary* registros = diccionario_registros(&proceso->registros);
	t_list* arrRegistros = dictionary_elements(registros);
	for (int i=0; i<12; i++) {
		switch (i/4) {
			case 0:
				memcpy(list_get(arrRegistros,i), list_remove(lista, 0), 4);
				break;
			case 1:
				memcpy(list_get(arrRegistros,i), list_remove(lista, 0), 8);
				break;
			case 2:
				memcpy(list_get(arrRegistros,i), list_remove(lista, 0), 16);
				break;
		}
	}
	dictionary_destroy(registros);
	*/
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
	dictionary_put(instrucciones, "EXIT", (void*)(intptr_t)EXIT);
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

void instruccion_set(t_dictionary* registros, char* parsed) {
	log_info(logger, "Instrucción SET.");
	parsed = strtok(NULL, " ");
	char* registro = dictionary_get(registros, parsed);
	parsed = strtok(NULL, " ");
	char* arg = parsed;
	memcpy(registro, arg, strlen(arg));
	delay(config_get_int_value(config, "RETARDO_INSTRUCCION"));
}

void instruccion_yield(t_dictionary* registros, char* parsed) {
	log_info(logger, "Instrucción YIELD.");
}

void instruccion_exit(t_dictionary* registros, char* parsed) {
	log_info(logger, "Instrucción EXIT.");
}

enum_instrucciones interpretar_instrucciones(pcb* proceso) {
	t_dictionary* instrucciones = diccionario_instrucciones();
	t_dictionary* registros = diccionario_registros(&proceso->registros);
	while (proceso->program_counter<list_size(proceso->instrucciones)) {
		char instruccion[64];
		char* get = list_get(proceso->instrucciones, proceso->program_counter);
		char* parsed;
		memcpy(instruccion, get, strlen(get));
		instruccion[strlen(get)] = '\0';
		parsed = strtok(instruccion, " ");
		switch ((int)(intptr_t)dictionary_get(instrucciones, parsed)) {
			case SET:
				instruccion_set(registros, parsed);
				break;
			case YIELD:
				// NO IMPLEMENTADA
				instruccion_yield(registros, parsed);
				log_debug(logger, "%c%c%c%c", proceso->registros.AX[0], proceso->registros.AX[1], proceso->registros.AX[2], proceso->registros.AX[3]);
				log_debug(logger, "%c%c%c%c%c%c%c%c", proceso->registros.ECX[0], proceso->registros.ECX[1], proceso->registros.ECX[2], proceso->registros.ECX[3], proceso->registros.ECX[4], proceso->registros.ECX[5], proceso->registros.ECX[6], proceso->registros.ECX[7]);
				log_debug(logger, "%c%c%c%c", proceso->registros.BX[0], proceso->registros.BX[1], proceso->registros.BX[2], proceso->registros.BX[3]);
				proceso->program_counter++;
				return YIELD;
			case EXIT:
				// NO IMPLEMENTADA
				instruccion_exit(registros, parsed);
				log_debug(logger, "E%c%c%c%c", proceso->registros.AX[0], proceso->registros.AX[1], proceso->registros.AX[2], proceso->registros.AX[3]);
				log_debug(logger, "E%c%c%c%c%c%c%c%c", proceso->registros.ECX[0], proceso->registros.ECX[1], proceso->registros.ECX[2], proceso->registros.ECX[3], proceso->registros.ECX[4], proceso->registros.ECX[5], proceso->registros.ECX[6], proceso->registros.ECX[7]);
				log_debug(logger, "E%c%c%c%c", proceso->registros.BX[0], proceso->registros.BX[1], proceso->registros.BX[2], proceso->registros.BX[3]);
				proceso->program_counter++;
				return EXIT;
		}
		proceso->program_counter++;
	}
	dictionary_destroy(instrucciones);
	dictionary_destroy(registros);
	return EXIT;
}
