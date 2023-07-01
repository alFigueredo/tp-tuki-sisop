#include "cliente.h"

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,
            server_info->ai_socktype,
            server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		log_error(logger, "¡No se pudo conectar al servidor!");
		abort();
	}
	freeaddrinfo(server_info);
	send_handshake(socket_cliente);
	log_info(logger, "¡Conectado al servidor!");
	return socket_cliente;
}

void send_handshake(int socket_cliente)
{
	uint32_t handshake = 1;
	uint32_t result;
	send(socket_cliente, &handshake, sizeof(uint32_t), 0);
	if (recv(socket_cliente, &result, sizeof(uint32_t), MSG_WAITALL) == -1 || result == -1) {
		log_error(logger, "¡Protocolo incompatible con el servidor!");
		abort();
	}
}

void esperar_servidor(int conexion){
	pthread_t thread;
	int *socket_servidor = malloc(sizeof(int));
	*socket_servidor = conexion;
	pthread_create(&thread,
	               NULL,
	               (void*) atender_servidor,
	               (void*) socket_servidor);
	pthread_detach(thread);
}

void atender_servidor(int* socket_servidor){
	t_list *lista;
	pthread_t thread;
	char* instruccion;
	char* numero = malloc(sizeof(char)*10); //No se me ocurre otra forma de hacerlo
	char** parsed;
	t_instruction* laCosaQueMando;
	Archivo archivoQueUso;
	while (1) {
		int cod_op = recibir_operacion(*socket_servidor);
		switch (cod_op) {
			case MENSAJE:
				recibir_mensaje(*socket_servidor);
				break;
			case PAQUETE:
				lista = recibir_paquete(*socket_servidor);
				log_info(logger, "Me llegaron los siguientes valores:");
				list_iterate(lista, (void*) iterator);
				list_destroy_and_destroy_elements(lista, free);
				break;
			case READY:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				exec_a_ready();
				list_destroy_and_destroy_elements(lista, free);
				break;
			case IO_BLOCK:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				pthread_create(&thread, NULL, (void*) io_block, NULL);
				pthread_detach(thread);
				list_destroy_and_destroy_elements(lista, free);
				break;
			case WAIT:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				manejo_recursos(((pcb*)queue_peek(qexec)), instruccion);
				list_destroy_and_destroy_elements(lista, free);
				break;
			case SIGNAL:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				manejo_recursos(((pcb*)queue_peek(qexec)), instruccion);
				list_destroy_and_destroy_elements(lista, free);
				break;
			case F_OPEN:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				if(abriArchivoKernel(((pcb*)queue_peek(qexec)), instruccion))
				{

				//PUEDO HACER ESTO?????
					laCosaQueMando = malloc(sizeof(t_instruction));
					laCosaQueMando->pid=((pcb*)queue_peek(qexec));
					laCosaQueMando->instruccion=instruccion;
					enviar_instruccion(*socket_servidor,laCosaQueMando,F_OPEN);
					free(laCosaQueMando);
				}
				list_destroy_and_destroy_elements(lista, free);
				break;
			case EL_ARCHIVO_NO_EXISTE_PAAAAAAA:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				//PUEDO HACER ESTO????? X2
				laCosaQueMando = malloc(sizeof(t_instruction));
				laCosaQueMando->pid=((pcb*)queue_peek(qexec));
				laCosaQueMando->instruccion=instruccion;
				enviar_instruccion(*socket_servidor,laCosaQueMando,F_CREATE);
				free(laCosaQueMando);

				list_destroy_and_destroy_elements(lista, free);
				break;
			case F_CLOSE:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				cerrarArchivoKernel(((pcb*)queue_peek(qexec)), instruccion);
				list_destroy_and_destroy_elements(lista, free);
				break;
			case F_SEEK:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				buscarEnArchivo(((pcb*)queue_peek(qexec)), instruccion);
				list_destroy_and_destroy_elements(lista, free);
				break;
			case F_TRUNCATE:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);

				//PUEDO HACER ESTO????? X2
				laCosaQueMando = malloc(sizeof(t_instruction));
				laCosaQueMando->pid=((pcb*)queue_peek(qexec));
				laCosaQueMando->instruccion=instruccion;
				enviar_instruccion(*socket_servidor,laCosaQueMando,F_TRUNCATE);
				free(laCosaQueMando);
				parsed = string_split(instruccion, " ");
				log_info(logger, "PID: %d - Archivo: %s - Tamaño: %d", laCosaQueMando->pid, parsed[1], parsed[2]);
				exec_a_block();
				list_destroy_and_destroy_elements(lista, free);
				break;
			case YA_SE_TERMINO_LA_TRUNCACION:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				block_a_ready();
				list_destroy_and_destroy_elements(lista, free);
				break;
			case F_READ:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				parsed = string_split(instruccion, " ");

				//esto deberia devolver el archivo que voy a usar
				archivoQueUso = estoDevuelveUnArchivo(((pcb*)queue_peek(qexec)), instruccion);

				sprintf(numero, "&d", archivoQueUso->puntero);

				//le meto el numero (como string) a la instruccion para mandarselo a file system
				strcat(instruccion," ");
				strcat(instruccion, numero);

				//PUEDO HACER ESTO????? X2
				laCosaQueMando = malloc(sizeof(t_instruction));
				laCosaQueMando->pid=((pcb*)queue_peek(qexec));
				laCosaQueMando->instruccion=instruccion;
				enviar_instruccion(*socket_servidor,laCosaQueMando,F_READ);
				free(laCosaQueMando);

				sem_wait(sem_escrituraLectura);
				contadorDeEscrituraOLectura ++;
				sem_post(sem_escrituraLectura);

				log_info(logger, "PID: %d - Leer Archivo: %s - Puntero: %d - Direccion Memoria %d - Tamaño %d", parsed[0], parsed[1], parsed[2], parsed[3], parsed[4]);
				exec_a_block();
				list_destroy_and_destroy_elements(lista, free);

				break;
			case MEMORIA_DIJO_QUE_PUDO_ESCRIBIR_JOYA:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				block_a_ready();

				sem_wait(sem_escrituraLectura);
				contadorDeEscrituraOLectura --;
				sem_post(sem_escrituraLectura);

				list_destroy_and_destroy_elements(lista, free);
				break;
			case F_WRITE:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				parsed = string_split(instruccion, " ");

				//esto deberia devolver el archivo que voy a usar
				archivoQueUso = estoDevuelveUnArchivo(((pcb*)queue_peek(qexec)), instruccion);

				sprintf(numero, "&d", archivoQueUso->puntero);

				//le meto el numero (como string) a la instruccion para mandarselo a file system
				strcat(instruccion," ");
				strcat(instruccion, numero);

				//PUEDO HACER ESTO????? X2
				laCosaQueMando = malloc(sizeof(t_instruction));
				laCosaQueMando->pid=((pcb*)queue_peek(qexec));
				laCosaQueMando->instruccion=instruccion;
				enviar_instruccion(*socket_servidor,laCosaQueMando,F_WRITE);
				free(laCosaQueMando);

				sem_wait(sem_escrituraLectura);
				contadorDeEscrituraOLectura ++;
				sem_post(sem_escrituraLectura);

				log_info(logger, "PID: %d - Escribir Archivo: %s - Puntero: %d - Direccion Memoria %d - Tamaño %d", parsed[0], parsed[1], parsed[2], parsed[3], parsed[4]);
				exec_a_block();
				list_destroy_and_destroy_elements(lista, free);

				break;
			case SE_PUDO_ESCRIBIR_EL_ARCHIVO:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				block_a_ready();

				sem_wait(sem_escrituraLectura);
				contadorDeEscrituraOLectura --;
				sem_post(sem_escrituraLectura);

				list_destroy_and_destroy_elements(lista, free);
				break;
			case CREATE_SEGMENT:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				enviar_segmento(instruccion, ((pcb*)queue_peek(qexec))->tabla_segmentos);
				list_destroy_and_destroy_elements(lista, free);
				
			case DELETE_SEGMENT:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, queue_peek(qexec));
				instruccion = list_get(((pcb*)queue_peek(qexec))->instrucciones, ((pcb*)queue_peek(qexec))->program_counter-1);
				enviar_segmento(instruccion, ((pcb*)queue_peek(qexec))->tabla_segmentos);
				
				list_destroy_and_destroy_elements(lista, free);
			case EXIT:
				lista = recibir_paquete(*socket_servidor);
				recibir_pcb(lista, (pcb*)queue_peek(qexec));
				exec_a_exit();
				enviar_pcb(conexion_memoria,(pcb*)queue_peek(qexec),EXIT);
				list_destroy_and_destroy_elements(lista, free);
				break;
			case -1:
				log_warning(logger, "El servidor se desconecto. Terminando conexion. Abortando sistema.");
				free(socket_servidor);
				abort();
				return;
			default:
				log_warning(logger,"Operacion desconocida. No quieras meter la pata");
				break;
		}
	}
}

void liberar_conexion(int socket_cliente)
{
	if (socket_cliente != -1) {
	log_warning(logger, "Liberando conexion");
	close(socket_cliente);
	}
}
