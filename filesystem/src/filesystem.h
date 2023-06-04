#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>
#include "cliente.h"
#include "server.h"
#include <commons/config.h>
#include <commons/log.h>
#include <commons/error.h>
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>
void terminar_programa(t_log*, t_config*);
void recorrerFCBs();
#endif /* FILESYSTEM_H_ */
