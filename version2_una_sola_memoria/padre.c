#include "comun.h"

int main(void) {
    const char *nombres[NUM_OPS] = {"suma", "resta", "multiplicacion", "division", "modulo"};
    const char *simbolos[NUM_OPS] = {"+", "-", "*", "/", "%"};
    pid_t pids[NUM_OPS];
    int a, b;

    printf("[PADRE] PID=%d (VERSION 2: una sola memoria compartida para todos)\n", (int)getpid());
    printf("Dame dos numeros enteros (a b): ");
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "Error: debes escribir dos numeros enteros.\n");
        return 1;
    }

    /* 1) Crear el unico segmento y escribir los operandos ANTES de crear a los hijos */
    int shmid = shmget(IPC_PRIVATE, sizeof(Datos), IPC_CREAT | 0666);
    if (shmid == -1) { perror("shmget"); return 1; }

    Datos *d = (Datos *)shmat(shmid, NULL, 0);
    if (d == (void *)-1) { perror("shmat"); shmctl(shmid, IPC_RMID, NULL); return 1; }

    memset(d, 0, sizeof(Datos));
    d->a = a;
    d->b = b;
    printf("[PADRE] memoria compartida creada: shmid=%d (%zu bytes)\n", shmid, sizeof(Datos));

    /* 2) Crear un hijo por operacion y reemplazarlo con exec por el programa de la operacion */
    fflush(stdout);
    for (int i = 0; i < NUM_OPS; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            shmdt(d);
            shmctl(shmid, IPC_RMID, NULL);
            return 1;
        }
        if (pids[i] == 0) {
            char ruta[64], idstr[16];
            snprintf(ruta, sizeof(ruta), "./%s", nombres[i]);
            snprintf(idstr, sizeof(idstr), "%d", shmid);
            /* exec descarta la imagen del hijo; por eso el shmid viaja como argumento */
            execl(ruta, nombres[i], idstr, (char *)NULL);
            perror("execl");
            _exit(1);
        }
        printf("[PADRE] hijo %-14s PID=%d\n", nombres[i], (int)pids[i]);
    }
    printf("[PADRE] Monitorea desde otra terminal con: ./monitorear.sh %d\n", (int)getpid());
    fflush(stdout);

    /* 3) Esperar a los 5 hijos */
    for (int i = 0; i < NUM_OPS; i++) {
        int estado;
        waitpid(pids[i], &estado, 0);
        if (!WIFEXITED(estado) || WEXITSTATUS(estado) != 0)
            printf("[PADRE] aviso: el hijo %s (PID=%d) no termino correctamente\n", nombres[i], (int)pids[i]);
    }

    /* 4) Leer los resultados de la memoria compartida */
    printf("\n========== RESULTADOS (a=%d, b=%d) ==========\n", a, b);
    for (int i = 0; i < NUM_OPS; i++) {
        if (!d->listo[i])        { printf("%-14s: el hijo no escribio resultado\n", nombres[i]); continue; }
        if (d->error[i])         { printf("%-14s: %d %s %d = indefinido (division entre cero)\n", nombres[i], a, simbolos[i], b); continue; }
        switch (i) {
            case OP_SUMA: printf("%-14s: %d %s %d = %lld\n", nombres[i], a, simbolos[i], b, d->suma); break;
            case OP_RESTA: printf("%-14s: %d %s %d = %lld\n", nombres[i], a, simbolos[i], b, d->resta); break;
            case OP_MULT: printf("%-14s: %d %s %d = %lld\n", nombres[i], a, simbolos[i], b, d->multiplicacion); break;
            case OP_DIV: printf("%-14s: %d %s %d = %.4f\n", nombres[i], a, simbolos[i], b, d->division); break;
            case OP_MOD: printf("%-14s: %d %s %d = %lld\n", nombres[i], a, simbolos[i], b, d->modulo); break;
        }
    }

    /* 5) Liberar: desenganchar y marcar el segmento para eliminarse */
    shmdt(d);
    shmctl(shmid, IPC_RMID, NULL);
    printf("[PADRE] segmento eliminado. Fin.\n");
    return 0;
}
