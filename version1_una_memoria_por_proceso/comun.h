#ifndef COMUN_H
#define COMUN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define NUM_OPS 5
#define ESPERA 30   /* segundos que cada proceso se queda vivo para poder monitorearlo */

/* VERSION 1: cada proceso hijo tiene su PROPIA memoria compartida con el padre.
   Este es el contenido de cada uno de esos 5 segmentos. */
typedef struct {
    int a;                 /* primer operando  (lo escribe el padre) */
    int b;                 /* segundo operando (lo escribe el padre) */
    long long res_entero;  /* resultado de suma, resta, multiplicacion y modulo */
    double res_real;       /* resultado de la division */
    int error;             /* 1 si hubo error (division entre cero) */
    int listo;             /* 1 cuando el hijo ya escribio su resultado */
} Mem;

#endif
