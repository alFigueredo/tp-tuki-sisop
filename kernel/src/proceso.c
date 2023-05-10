#include "proceso.h"

t_queue* qnew;
t_queue* qready;
t_queue* qexec;
t_queue* qblock;
t_queue* qexit;

void iniciar_colas(void) {
	qnew = queue_create();
	qready = queue_create();
	qexec = queue_create();
	qblock = queue_create();
	qexit = queue_create();
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

void enviar_pcb_a_cpu(int conexion_cpu, pcb* proceso) {
	t_paquete* paquete = crear_paquete(EXEC);
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
	enviar_paquete(paquete, conexion_cpu);
	eliminar_paquete(paquete);
}

pcb* recibir_pcb_de_cpu(t_list* lista) {
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
