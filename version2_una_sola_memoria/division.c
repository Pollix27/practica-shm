#include "comun.h"

/* Proceso hijo: division. Recibe por argumento el shmid de la memoria compartida UNICA. */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <shmid>\n", argv[0]);
        return 1;
    }
    int shmid = atoi(argv[1]);

    Datos *d = (Datos *)shmat(shmid, NULL, 0);
    if (d == (void *)-1) { perror("shmat"); return 1; }

    printf("[DIVISION] PID=%d PPID=%d shmid=%d -> leo a=%d b=%d\n",
           (int)getpid(), (int)getppid(), shmid, d->a, d->b);

    if (d->b == 0) d->error[OP_DIV] = 1;
    else            d->division = (double)d->a / (double)d->b;
    d->listo[OP_DIV] = 1;
    printf("[DIVISION] PID=%d escribi mi resultado en mi campo de la memoria compartida\n", (int)getpid());

    sleep(ESPERA);   /* para dar tiempo de monitorear */
    shmdt(d);
    printf("[DIVISION] PID=%d terminando\n", (int)getpid());
    return 0;
}
