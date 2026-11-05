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

/* indices de cada operacion */
enum { OP_SUMA, OP_RESTA, OP_MULT, OP_DIV, OP_MOD };

/* VERSION 2: UNA SOLA memoria compartida entre el padre y los 5 procesos.
   Cada proceso escribe unicamente en su propio campo, por eso no hay choques. */
typedef struct {
    int a;                    /* primer operando  (lo escribe el padre) */
    int b;                    /* segundo operando (lo escribe el padre) */
    long long suma;           /* resultados, uno por proceso */
    long long resta;
    long long multiplicacion;
    double division;
    long long modulo;
    int error[NUM_OPS];       /* 1 si esa operacion fallo (division/modulo entre cero) */
    int listo[NUM_OPS];       /* 1 cuando esa operacion ya escribio su resultado */
} Datos;

#endif
