#include "proceso.h"

t_queue* qnew;
t_queue* qready;
t_queue* qexit;

pcb* generar_proceso(t_list* lista) {
	pcb* proceso = malloc(sizeof(pcb));
	proceso->pid = process_getpid();
	proceso->instrucciones=list_duplicate(lista);
	proceso->program_counter=0;
	//proceso.estimado_proxRafaga= config_get_in t_value(config,"ESTIMACION_INICIAL");
	log_info(logger, "Se crea el proceso %d en NEW", proceso->pid);
	return proceso;
}
