#ifndef LOGCONFIG_H_
#define LOGCONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

extern t_log* logger;
extern t_config* config;

t_log* iniciar_logger(char*);
t_config* iniciar_config(char*);

#endif /* LOGCONFIG_H_ */
