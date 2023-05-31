#include "proceso.h"

int conexion_cpu;
int conexion_memoria;
int conexion_filesystem;
t_queue* qnew;
t_queue* qready;
t_queue* qexec;
t_queue* q_block;
t_queue* qexit;
sem_t* sem_largo_plazo;
sem_t* sem_cpu;
sem_t* sem_new;
sem_t* sem_ready;
sem_t* sem_exec;
sem_t* sem_block;
sem_t* sem_exit;
sem_t* sem_new_ready;
sem_t* sem_exec_exit;
t_temporal* tiempo_en_cpu;
t_dictionary* conexiones;

void iniciar_colas(void) {
	qnew = queue_create();
	qready = queue_create();
	qexec = queue_create();
	q_block = queue_create();
	qexit = queue_create();
}

void destruir_colas(void) {
	queue_destroy(qnew);
	queue_destroy(qready);
	queue_destroy(qexec);
	queue_destroy(q_block);
	queue_destroy(qexit);
}

void iniciar_semaforos(void) {
	sem_largo_plazo = iniciar_semaforo(0, config_get_int_value(config, "GRADO_MAX_MULTIPROGRAMACION"));
	sem_cpu = iniciar_semaforo(0, 1);
	sem_new = iniciar_semaforo(0, 1);
	sem_ready = iniciar_semaforo(0, 1);
	sem_exec = iniciar_semaforo(0, 1);
	sem_block = iniciar_semaforo(0, 1);
	sem_exit = iniciar_semaforo(0, 1);
	sem_new_ready = iniciar_semaforo(0, 0);
	sem_exec_exit = iniciar_semaforo(0, 0);
}

void destruir_semaforos(void) {
	destruir_semaforo(sem_largo_plazo);
	destruir_semaforo(sem_cpu);
	destruir_semaforo(sem_new);
	destruir_semaforo(sem_ready);
	destruir_semaforo(sem_exec);
	destruir_semaforo(sem_block);
	destruir_semaforo(sem_exit);
	destruir_semaforo(sem_new_ready);
	destruir_semaforo(sem_exec_exit);
}

sem_t* iniciar_semaforo(int pshared, unsigned int value) {
	sem_t* semaforo = malloc(sizeof(sem_t));
	sem_init(semaforo, pshared, value);
	return semaforo;
}

void destruir_semaforo(sem_t* semaforo) {
	sem_destroy(semaforo);
	free(semaforo);
}

void delay(int milliseconds)
{
	t_temporal* clock = temporal_create();
	while(temporal_gettime(clock)<milliseconds)
		;
	temporal_destroy(clock);
}

void queue_extract(t_queue* queue, pcb* proceso) {
	int size_queue = queue_size(queue);
	for (int i=0; i<size_queue; i++) {
		if (((pcb*)queue_peek(queue))->pid==proceso->pid) {
			queue_pop(queue);
		} else {
			queue_push(queue, queue_pop(queue));
		}
	}
}

void generar_proceso(t_list* lista, int* socket_cliente) {
	pcb* proceso = malloc(sizeof(pcb));
	memcpy(&(proceso->pid), list_remove(lista, 0), sizeof(unsigned int));
	proceso->instrucciones=list_duplicate(lista);
	proceso->program_counter=0;
	proceso->estimado_proxRafaga=config_get_int_value(config,"ESTIMACION_INICIAL");
	sem_wait(sem_new);
	queue_push(qnew, proceso);
	sem_post(sem_new);
	dictionary_put(conexiones, string_itoa(proceso->pid), socket_cliente);
	log_info(logger, "Se crea el proceso %d en NEW", proceso->pid);
}

void new_a_ready(void) {
	sem_wait(sem_largo_plazo);
	// Si el ingreso del proceso coincide con la salida de otro
	int sem_new_ready_value;
	sem_getvalue(sem_new_ready, &sem_new_ready_value);
	if (sem_new_ready_value==1)
		sem_wait(sem_new_ready);
	sem_wait(sem_new);
	pcb* proceso = queue_pop(qnew);
	sem_post(sem_new);
	sem_wait(sem_ready);
	queue_push(qready, proceso);
	sem_post(sem_ready);
	log_info(logger, "PID: %d - Estado Anterior: NEW - Estado Actual: READY", proceso->pid);
	proceso->tiempo_llegada_ready = temporal_get_string_time("%y:%m:%d:%H:%M:%S:%MS");
	sem_wait(sem_ready);
	log_info(logger, "Cola Ready %s: [%s]", config_get_string_value(config,"ALGORITMO_PLANIFICACION"), queue_iterator(qready));
	sem_post(sem_ready);
	if (sem_new_ready_value==1)
		sem_post(sem_exec_exit);
	ready_a_exec();
}

void ready_a_exec(void) {
	sem_wait(sem_cpu);
	sem_wait(sem_ready);
	if (strcmp(config_get_string_value(config,"ALGORITMO_PLANIFICACION"), "HRRN")==0) {
		planificador(qready);
		log_info(logger, "Cola Ready %s: [%s]", config_get_string_value(config,"ALGORITMO_PLANIFICACION"), queue_iterator(qready));
	}
	pcb* proceso = queue_pop(qready);
	sem_post(sem_ready);
	queue_push(qexec, proceso);
	log_info(logger, "PID: %d - Estado Anterior: READY - Estado Actual: EXEC", proceso->pid);
	tiempo_en_cpu = temporal_create();
	enviar_pcb(conexion_cpu, queue_peek(qexec), EXEC);
}

void exec_a_ready(void) {
	pcb* proceso = queue_pop(qexec);
	sem_wait(sem_ready);
	queue_push(qready, proceso);
	sem_post(sem_ready);
	log_info(logger, "PID: %d - Estado Anterior: EXEC - Estado Actual: READY", proceso->pid);
	calcular_estimacion(proceso, temporal_gettime(tiempo_en_cpu));
	temporal_destroy(tiempo_en_cpu);
	proceso->tiempo_llegada_ready = temporal_get_string_time("%y:%m:%d:%H:%M:%S:%MS");
	sem_wait(sem_ready);
	log_info(logger, "Cola Ready %s: [%s]", config_get_string_value(config,"ALGORITMO_PLANIFICACION"), queue_iterator(qready));
	sem_post(sem_ready);
	sem_post(sem_cpu);
	pthread_t thread;
	pthread_create(&thread, NULL, (void*) ready_a_exec, NULL);
	pthread_detach(thread);
}

pcb* exec_a_block() {
	pcb* proceso = queue_pop(qexec);
	sem_wait(sem_block);
	queue_push(q_block, proceso);
	sem_post(sem_block);
	log_info(logger, "PID: %d - Estado Anterior: EXEC - Estado Actual: BLOCK", proceso->pid);
	calcular_estimacion(proceso, temporal_gettime(tiempo_en_cpu));
	temporal_destroy(tiempo_en_cpu);
	sem_post(sem_cpu);
	return proceso;
}

void block_a_ready(pcb* proceso) {
	sem_wait(sem_block);
	queue_extract(q_block, proceso);
	sem_post(sem_block);
	sem_wait(sem_ready);
	queue_push(qready, proceso);
	sem_post(sem_ready);
	log_info(logger, "PID: %d - Estado Anterior: BLOCK - Estado Actual: READY", proceso->pid);
	proceso->tiempo_llegada_ready = temporal_get_string_time("%y:%m:%d:%H:%M:%S:%MS");
	sem_wait(sem_ready);
	log_info(logger, "Cola Ready %s: [%s]", config_get_string_value(config,"ALGORITMO_PLANIFICACION"), queue_iterator(qready));
	sem_post(sem_ready);
	ready_a_exec();
}

void exec_a_exit() {
	pcb* proceso = queue_pop(qexec);
	queue_push(qexit, proceso);
	log_info(logger, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", proceso->pid);
	calcular_estimacion(proceso, temporal_gettime(tiempo_en_cpu));
	temporal_destroy(tiempo_en_cpu);

//	log_trace(logger, "Registro AX: %s", string_substring_until(proceso->registros.AX, 4));
//	log_trace(logger, "Registro BX: %s", string_substring_until(proceso->registros.BX, 4));
//	log_trace(logger, "Registro CX: %s", string_substring_until(proceso->registros.CX, 4));
//	log_trace(logger, "Registro DX: %s", string_substring_until(proceso->registros.DX, 4));
//	log_trace(logger, "Registro EAX: %s", string_substring_until(proceso->registros.EAX, 8));
//	log_trace(logger, "Registro EBX: %s", string_substring_until(proceso->registros.EBX, 8));
//	log_trace(logger, "Registro ECX: %s", string_substring_until(proceso->registros.ECX, 8));
//	log_trace(logger, "Registro EDX: %s", string_substring_until(proceso->registros.EDX, 8));
//	log_trace(logger, "Registro RAX: %s", string_substring_until(proceso->registros.RAX, 16));
//	log_trace(logger, "Registro RBX: %s", string_substring_until(proceso->registros.RBX, 16));
//	log_trace(logger, "Registro RCX: %s", string_substring_until(proceso->registros.RCX, 16));
//	log_trace(logger, "Registro RDX: %s", string_substring_until(proceso->registros.RDX, 16));

	enviar_operacion(*(int*)dictionary_remove(conexiones, string_itoa(proceso->pid)), EXIT);
	log_info(logger, "Finaliza el proceso %d - Motivo: SUCCESS", proceso->pid);
	// Si hay procesos en NEW esperando
	int sem_largo_plazo_value;
	sem_getvalue(sem_largo_plazo, &sem_largo_plazo_value);
	if (sem_largo_plazo_value==0 && queue_size(qnew)!=0) {
		sem_post(sem_largo_plazo);
		sem_post(sem_new_ready);
		sem_wait(sem_exec_exit);
	} else {
		sem_post(sem_largo_plazo);
	}
	sem_post(sem_cpu);
	free(queue_pop(qexit));
}

void enviar_pcb(int conexion, pcb* proceso, op_code codigo) {
	t_paquete* paquete = crear_paquete(codigo);

	agregar_a_paquete(paquete, &(proceso->pid), sizeof(unsigned int));

	int cantidad_instrucciones = list_size(proceso->instrucciones);
	agregar_a_paquete(paquete, &cantidad_instrucciones, sizeof(int));

	for (int i=0; i<cantidad_instrucciones; i++) {
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

	agregar_a_paquete(paquete, &(proceso->estimado_proxRafaga), sizeof(int));
	agregar_a_paquete(paquete, &(proceso->tiempo_llegada_ready), strlen(proceso->tiempo_llegada_ready)+1);

	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
}

void recibir_pcb(t_list* lista, pcb* proceso) {
	memcpy(&(proceso->pid), list_remove(lista, 0), sizeof(unsigned int));
	int cantidad_instrucciones;
	memcpy(&(cantidad_instrucciones), list_remove(lista, 0), sizeof(int));
	proceso->instrucciones = list_take_and_remove(lista, cantidad_instrucciones);
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
	memcpy(&proceso->estimado_proxRafaga, list_remove(lista, 0), sizeof(int));
	proceso->tiempo_llegada_ready = (char*)list_remove(lista, 0);
}

//void planificador(t_queue* queue) {
//	t_list* list = list_create();
//	while (queue_size(queue)!=0) {
//		list_add(list, queue_pop(queue));
//	}
//	list_sort(list, HRRN_comparator);
//	while (list_size(list)!=0) {
//		queue_push(queue, list_remove(list, 0));
//	}
//}

void planificador(t_queue* queue) {
	t_list* list = list_create();
	while (queue_size(queue)!=0) {
		list_add_sorted(list, queue_pop(queue), HRRN_comparator);
	}
	while (list_size(list)!=0) {
		queue_push(queue, list_remove(list, 0));
	}
}

void calcular_estimacion(pcb* proceso, int64_t tiempo_transcurrido) {
	double alfa = config_get_double_value(config,"HRRN_ALFA");
	proceso->estimado_proxRafaga=alfa*tiempo_transcurrido+proceso->estimado_proxRafaga*(1-alfa);
}

bool HRRN_comparator(void* proceso1, void* proceso2) {
	double r1 = HRRN_R((pcb*)proceso1);
	double r2 = HRRN_R((pcb*)proceso2);
	return r1>=r2;
}

double HRRN_R(pcb* proceso) {
	int arrival_time = seconds_from_string_time(proceso->tiempo_llegada_ready);
	int current_time = seconds_from_string_time(temporal_get_string_time("%y:%m:%d:%H:%M:%S:%MS"));
	return (double)(current_time-arrival_time+proceso->estimado_proxRafaga)/proceso->estimado_proxRafaga;
}

int seconds_from_string_time(char* timestamp) {
	char** ts_sorted = string_split(timestamp, ":");
	// Estimado, no tiene en cuenta los años y todos los meses tienen 31 días
	int datetime = (atoi(ts_sorted[1]))*31+atoi(ts_sorted[2]);
	return ((datetime*24+atoi(ts_sorted[3])*60+atoi(ts_sorted[4]))*60+atoi(ts_sorted[5]))*1000+atoi(ts_sorted[6]);
}

void io_block(void) {
	pcb* proceso = exec_a_block();
	log_info(logger, "PID: %d - Bloqueado por: IO", proceso->pid);
	char* instruccion = list_get(proceso->instrucciones, proceso->program_counter-1);
	char** parsed = string_split(instruccion, " ");
	// Supuse que el parámetro está en segundos
	int delay_in_seconds = atoi(parsed[1]);
	log_info(logger, "PID: %d - Ejecuta IO: %d", proceso->pid, delay_in_seconds);
	delay(delay_in_seconds*1000);
	block_a_ready(proceso);
}

char* queue_iterator(t_queue* queue) {
	char* list = string_new();
	for (int i=0; i<queue_size(queue); i++) {
		pcb* proceso = queue_pop(queue);
		string_append(&list, string_itoa(proceso->pid));
		if (i!=queue_size(queue))
			string_append(&list, ",");
		queue_push(queue, proceso);
	}
	return list;
}
