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
#include <ftw.h>

extern char **vectorDePathsPCBs;
extern t_config* superBloque;
extern t_bitarray *bitmap;

void terminar_programa(t_log*, t_config*,char**);
int recorrerFCBs();
char* concatenarCadenas(const char* str1, const char* str2);
int contarArchivosEnCarpeta(const char *carpeta, char ***vectoreRutas);
int abrirArchivo(char *nombre, char **vectorDePaths,int cantidadPaths);
int crearArchivo(char *nombre,char *carpeta, char ***vectoreRutas, int *cantidadPaths);
int truncarArchivo(char *nombre,char *carpeta, char ***vectoreRutas, int *cantidadPaths, int tamanioNuevo);
int dividirRedondeando(int numero1 , int numero2);
void sacarBloques(int cantidadBloquesOriginal ,int cantidadBloquesNueva,t_config* configArchivoActual,int tamanioOriginal);
void agregarBloques(int cantidadBloquesOriginal ,int cantidadBloquesNueva,t_config* configArchivoActual);
#endif /* FILESYSTEM_H_ */
