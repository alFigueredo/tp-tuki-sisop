#ifndef CONSOLA_H_
#define CONSOLA_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "cliente.h"


t_log* iniciar_logger(void);
t_config* iniciar_config(char*);
void leer_consola(t_log*);
void paquete(int);
void paquete_texto(int, char*);
void terminar_programa(int, t_log*, t_config*);

#endif /* CONSOLA_H_ */
