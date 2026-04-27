# 🧠 TUKI – Simulación de Sistema Operativo Distribuido

- [Enunciado](./TUKI-v1.1.pdf)
- [Pruebas](./TUKI-Pruebas-v1.2.pdf)

**TUKI (The Ultimate Kernel Implementation)** es un sistema distribuido que simula el funcionamiento de un sistema operativo, incluyendo planificación de procesos, gestión de memoria y sistema de archivos.

## 🚀 Descripción

El sistema está compuesto por múltiples módulos que interactúan entre sí mediante sockets TCP/IP, replicando la arquitectura de un sistema operativo real:

* **Kernel**: planificación de procesos y gestión de recursos
* **CPU**: ejecución de instrucciones (ciclo Fetch–Decode–Execute)
* **Memoria**: administración de memoria segmentada
* **File System**: persistencia de datos y gestión de archivos
* **Consola**: generación de procesos e instrucciones

## ⚙️ Características principales

* Planificación de procesos (**FIFO** y **HRRN**)
* Manejo de concurrencia y sincronización
* Gestión de recursos y detección de condiciones de bloqueo
* Simulación de memoria con segmentación y compactación
* Implementación de un sistema de archivos con persistencia
* Comunicación entre módulos mediante **sockets TCP/IP**
* Sistema configurable mediante archivos `.config`

## 🛠️ Tecnologías

* **C (GNU/Linux)**
* **Sockets**
* **Multithreading**
* **Makefiles**
* **Logs y configuración por archivos**

## 📌 Notas

Proyecto académico desarrollado en el marco de la materia Sistemas Operativos (UTN), enfocado en la implementación práctica de conceptos fundamentales de sistemas.
