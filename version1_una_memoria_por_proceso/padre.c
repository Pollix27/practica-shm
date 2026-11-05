#include "comun.h"

/* Desengancha y elimina los segmentos creados hasta ahora. */
static void liberar(int ids[], Mem *mem[], int n) {
    for (int i = 0; i < n; i++) {
        if (mem[i] != NULL && mem[i] != (void *)-1) shmdt(mem[i]);
        if (ids[i] != -1) shmctl(ids[i], IPC_RMID, NULL);
    }
}

int main(void) {
    const char *nombres[NUM_OPS] = {"suma", "resta", "multiplicacion", "division", "modulo"};
    const char *simbolos[NUM_OPS] = {"+", "-", "*", "/", "%"};
    int ids[NUM_OPS];
    Mem *mem[NUM_OPS];
    pid_t pids[NUM_OPS];
    int a, b;

    printf("[PADRE] PID=%d (VERSION 1: una memoria compartida por cada hijo)\n", (int)getpid());
    printf("Dame dos numeros enteros (a b): ");
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "Error: debes escribir dos numeros enteros.\n");
        return 1;
    }

    for (int i = 0; i < NUM_OPS; i++) { ids[i] = -1; mem[i] = NULL; }

    /* 1) Crear un segmento por operacion y escribir los operandos ANTES de crear a los hijos */
    for (int i = 0; i < NUM_OPS; i++) {
        ids[i] = shmget(IPC_PRIVATE, sizeof(Mem), IPC_CREAT | 0666);
        if (ids[i] == -1) { perror("shmget"); liberar(ids, mem, NUM_OPS); return 1; }

        mem[i] = (Mem *)shmat(ids[i], NULL, 0);
        if (mem[i] == (void *)-1) { perror("shmat"); liberar(ids, mem, NUM_OPS); return 1; }

        memset(mem[i], 0, sizeof(Mem));
        mem[i]->a = a;
        mem[i]->b = b;
        printf("[PADRE] segmento para %-14s shmid=%d\n", nombres[i], ids[i]);
    }

    /* 2) Crear un hijo por operacion y reemplazarlo con exec por el programa de la operacion */
    fflush(stdout);
    for (int i = 0; i < NUM_OPS; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            liberar(ids, mem, NUM_OPS);
            return 1;
        }
        if (pids[i] == 0) {
            char ruta[64], idstr[16];
            snprintf(ruta, sizeof(ruta), "./%s", nombres[i]);
            snprintf(idstr, sizeof(idstr), "%d", ids[i]);
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

    /* 4) Leer los resultados de cada memoria compartida */
    printf("\n========== RESULTADOS (a=%d, b=%d) ==========\n", a, b);
    for (int i = 0; i < NUM_OPS; i++) {
        if (!mem[i]->listo)            printf("%-14s: el hijo no escribio resultado\n", nombres[i]);
        else if (mem[i]->error)        printf("%-14s: %d %s %d = indefinido (division entre cero)\n", nombres[i], a, simbolos[i], b);
        else if (i == 3)               printf("%-14s: %d %s %d = %.4f\n", nombres[i], a, simbolos[i], b, mem[i]->res_real);
        else                           printf("%-14s: %d %s %d = %lld\n", nombres[i], a, simbolos[i], b, mem[i]->res_entero);
    }

    /* 5) Liberar: desenganchar y marcar los segmentos para eliminarse */
    liberar(ids, mem, NUM_OPS);
    printf("[PADRE] segmentos eliminados. Fin.\n");
    return 0;
}
