#include "proceso.h"

int conexion_cpu;
int conexion_memoria;
int conexion_filesystem;
t_queue* qnew;
t_queue* qready;
t_queue* qexec;
t_queue* qblock;
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
	qblock = queue_create();
	qexit = queue_create();
}

void destruir_colas(void) {
	queue_destroy(qnew);
	queue_destroy(qready);
	queue_destroy(qexec);
	queue_destroy(qblock);
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
	queue_push(qblock, proceso);
	sem_post(sem_block);
	log_info(logger, "PID: %d - Estado Anterior: EXEC - Estado Actual: BLOCK", proceso->pid);
	calcular_estimacion(proceso, temporal_gettime(tiempo_en_cpu));
	temporal_destroy(tiempo_en_cpu);
	sem_post(sem_cpu);
	return proceso;
}

void block_a_ready(pcb* proceso) {
	sem_wait(sem_block);
	queue_extract(qblock, proceso);
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

//	log_trace(logger, "Registro RAX: %s", string_substring_until(proceso->registros.RAX, 16));

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
	proceso->estimado_proxRafaga=alfa*tiempo_transcurrido+proceso->estimado_proxRafaga*(1.0-alfa);
}

bool HRRN_comparator(void* proceso1, void* proceso2) {
	double r1 = HRRN_R((pcb*)proceso1);
	double r2 = HRRN_R((pcb*)proceso2);
	// log_trace(logger, "PID1: %d, R1: %f; PID2: %d, R2: %f", ((pcb*)proceso1)->pid, r1, ((pcb*)proceso2)->pid, r2);
	return r1>=r2;
}

double HRRN_R(pcb* proceso) {
	int arrival_time = seconds_from_string_time(proceso->tiempo_llegada_ready);
	int current_time = seconds_from_string_time(temporal_get_string_time("%y:%m:%d:%H:%M:%S:%MS"));
	// log_trace(logger, "HRRN: %d; Estimación: %f", proceso->pid, (double)(current_time-arrival_time+proceso->estimado_proxRafaga)/proceso->estimado_proxRafaga);
	// log_trace(logger, "PID: %d; HRRN: %d %d", proceso->pid, current_time-arrival_time, proceso->estimado_proxRafaga);
	return (double)(current_time-arrival_time+proceso->estimado_proxRafaga)/proceso->estimado_proxRafaga;
}

int seconds_from_string_time(char* timestamp) {
	char** ts_sorted = string_split(timestamp, ":");
	// Estimado
	int datetime = atoi(ts_sorted[0])/4+(atoi(ts_sorted[0])-21)*365+month_days(atoi(ts_sorted[1])-1)+atoi(ts_sorted[2]-1);
	// log_trace(logger, "Timestamp: %s, Datetime: %d", timestamp, ((datetime*24+atoi(ts_sorted[3])*60+atoi(ts_sorted[4]))*60+atoi(ts_sorted[5]))*1000+atoi(ts_sorted[6]));
	return ((datetime*24+atoi(ts_sorted[3])*60+atoi(ts_sorted[4]))*60+atoi(ts_sorted[5]))*1000+atoi(ts_sorted[6]);
}

int month_days(int month) {
	int days_amount[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int sum = 0;
	for (int i=0; i<month; i++)
		sum += days_amount[i];
	return sum;
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
