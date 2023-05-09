#include "proceso.h"

t_queue* qnew;
t_queue* qready;
t_queue* qexec;
t_queue* qblock;
t_queue* qexit;

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

pcb* generar_proceso(t_list* lista) {
	pcb* proceso = malloc(sizeof(pcb));
	proceso->pid = process_getpid();
	proceso->instrucciones=list_duplicate(lista);
	proceso->program_counter=0;
	//proceso.estimado_proxRafaga= config_get_in t_value(config,"ESTIMACION_INICIAL");
	log_info(logger, "Se crea el proceso %d en NEW", proceso->pid);
	return proceso;
}
