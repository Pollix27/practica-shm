#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Estructura para pasar información y recibir resultados de cada hilo (siguiendo el estilo de tus prácticas)
typedef struct operacion {
    int id;
    int a;
    int b;
    int tipo; // 0: suma, 1: resta, 2: multiplicación, 3: división, 4: módulo
    int resultado_int;
    float resultado_float;
    int estado; // 1 si es exitoso, 0 si hay error (ej. división entre cero)
} Operacion;

// Prototipos de funciones
void *funcion_calculadora(void *arg);
Operacion *crearInfoDinamicos(int numHilos);
pthread_t *crearHilosDinamic(int numHilos);

int main(int argc, char *argv[]) {
    int numHilos = 5; // 5 hilos dinámicos para las 5 operaciones básicas
    pthread_t *hilos;
    Operacion *infoHilos;
    int a, b;

    // Validar argumentos de entrada desde la terminal (ej: ./calculadora 12 5)
    if (argc < 3) {
        printf("Faltan argumentos. Uso: %s <operando1> <operando2>\n", argv[0]);
        exit(1);
    }

    a = atoi(argv[1]);
    b = atoi(argv[2]);

    printf("========================================\n");
    printf("   CALCULADORA CON HILOS DINÁMICOS (POSIX)\n");
    printf("========================================\n");
    printf("Operandos ingresados: a = %d, b = %d\n\n", a, b);

    // Creación dinámica de estructuras y hilos (idéntico al enfoque de tus ejemplos)
    infoHilos = crearInfoDinamicos(numHilos);
    hilos = crearHilosDinamic(numHilos);

    // Nombres de las operaciones para mostrar en pantalla
    char *nombres_ops[] = {"Suma", "Resta", "Multiplicación", "División", "Módulo"};

    // Inicializar los datos que recibirá cada hilo
    for (int i = 0; i < numHilos; i++) {
        infoHilos[i].id = i;
        infoHilos[i].a = a;
        infoHilos[i].b = b;
        infoHilos[i].tipo = i;
        infoHilos[i].estado = 1;
    }

    // Creación e inicio de hilos dinámicamente con pthread_create
    for (int i = 0; i < numHilos; i++) {
        pthread_create(&hilos[i], NULL, funcion_calculadora, (void *)&infoHilos[i]);
    }

    // Esperar a que todos los hilos terminen su ejecución con pthread_join
    for (int i = 0; i < numHilos; i++) {
        pthread_join(hilos[i], NULL);
    }

    // Mostrar los resultados consolidados desde el hilo principal (main)
    printf("--- RESULTADOS CALCULADOS POR LOS HILOS ---\n");
    for (int i = 0; i < numHilos; i++) {
        if (infoHilos[i].tipo == 3) { // División
            if (infoHilos[i].estado) {
                printf("Hilo %d [%-15s] -> %d / %d = %.2f\n", 
                       infoHilos[i].id, nombres_ops[i], a, b, infoHilos[i].resultado_float);
            } else {
                printf("Hilo %d [%-15s] -> Error: División entre cero\n", 
                       infoHilos[i].id, nombres_ops[i]);
            }
        } else if (infoHilos[i].tipo == 4) { // Módulo
            if (infoHilos[i].estado) {
                printf("Hilo %d [%-15s] -> %d %% %d = %d\n", 
                       infoHilos[i].id, nombres_ops[i], a, b, infoHilos[i].resultado_int);
            } else {
                printf("Hilo %d [%-15s] -> Error: Módulo por cero\n", 
                       infoHilos[i].id, nombres_ops[i]);
            }
        } else { // Suma, Resta, Multiplicación
            char simbolo = '+';
            if (infoHilos[i].tipo == 1) simbolo = '-';
            if (infoHilos[i].tipo == 2) simbolo = '*';
            
            printf("Hilo %d [%-15s] -> %d %c %d = %d\n", 
                   infoHilos[i].id, nombres_ops[i], a, simbolo, b, infoHilos[i].resultado_int);
        }
    }
    printf("========================================\n");

    // Liberar la memoria dinámica reservada
    free(hilos);
    free(infoHilos);

    return 0;
}

// Funciones de asignación dinámica de memoria (estilo hiloscoperativosIndices.c)
Operacion *crearInfoDinamicos(int numHilos) {
    return (Operacion *)calloc(numHilos, sizeof(Operacion));
}

pthread_t *crearHilosDinamic(int numHilos) {
    return (pthread_t *)calloc(numHilos, sizeof(pthread_t));
}

// Función ejecutada por cada hilo dinámico
void *funcion_calculadora(void *arg) {
    Operacion *op = (Operacion *)arg;

    // Realizar la operación según el tipo asignado
    switch (op->tipo) {
        case 0: // Suma
            op->resultado_int = op->a + op->b;
            break;
        case 1: // Resta
            op->resultado_int = op->a - op->b;
            break;
        case 2: // Multiplicación
            op->resultado_int = op->a * op->b;
            break;
        case 3: // División
            if (op->b != 0) {
                op->resultado_float = (float)op->a / op->b;
            } else {
                op->estado = 0; // Marcar error de división entre cero
            }
            break;
        case 4: // Módulo
            if (op->b != 0) {
                op->resultado_int = op->a % op->b;
            } else {
                op->estado = 0; // Marcar error de módulo entre cero
            }
            break;
    }

    pthread_exit(NULL);
}