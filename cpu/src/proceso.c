#include "proceso.h"

pcb* recibir_pcb_de_kernel(t_list* lista) {
	pcb* proceso = malloc(sizeof(pcb));
	memcpy(&(proceso->pid), list_get(lista, 0), sizeof(unsigned int));
	proceso->instrucciones = list_slice(lista, 1, list_size(lista)-2);
	memcpy(&(proceso->program_counter), list_get(lista, list_size(lista)-1), sizeof(int));
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
	for (int i=0; i<strlen(arg); i++)
		registro[i] = arg[i];
	delay(config_get_int_value(config, "RETARDO_INSTRUCCION"));
}

void instruccion_yield(t_dictionary* registros, char* parsed) {
	log_info(logger, "Instrucción YIELD.");
}

void instruccion_exit(t_dictionary* registros, char* parsed) {
	log_info(logger, "Instrucción EXIT.");
}

void interpretar_instruccion(pcb* proceso) {
	t_dictionary* instrucciones = diccionario_instrucciones();
	t_dictionary* registros = diccionario_registros(&proceso->registros);
	while (proceso->program_counter<list_size(proceso->instrucciones)) {
		char* get = list_get(proceso->instrucciones, proceso->program_counter);
		char instruccion[64];
		char* parsed;
		memcpy(instruccion, get, strlen(get));
		parsed = strtok(instruccion, " ");
		switch ((int)(intptr_t)dictionary_get(instrucciones, parsed)) {
			case SET:
				instruccion_set(registros, parsed);
				break;
			case YIELD:
				// NO IMPLEMENTADA
				instruccion_yield(registros, parsed);
				break;
			case EXIT:
				// NO IMPLEMENTADA
				instruccion_exit(registros, parsed);
				break;
		}
		proceso->program_counter++;
	}
	log_debug(logger, "%c%c%c%c", proceso->registros.AX[0], proceso->registros.AX[1], proceso->registros.AX[2], proceso->registros.AX[3]);
	log_debug(logger, "%c%c%c%c", proceso->registros.ECX[0], proceso->registros.ECX[2], proceso->registros.ECX[4], proceso->registros.ECX[5]);
	dictionary_destroy(instrucciones);
	dictionary_destroy(registros);
}
