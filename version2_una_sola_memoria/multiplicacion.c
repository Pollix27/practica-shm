#include "comun.h"

/* Proceso hijo: multiplicacion. Recibe por argumento el shmid de la memoria compartida UNICA. */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <shmid>\n", argv[0]);
        return 1;
    }
    int shmid = atoi(argv[1]);

    Datos *d = (Datos *)shmat(shmid, NULL, 0);
    if (d == (void *)-1) { perror("shmat"); return 1; }

    printf("[MULTIPLICACION] PID=%d PPID=%d shmid=%d -> leo a=%d b=%d\n",
           (int)getpid(), (int)getppid(), shmid, d->a, d->b);

    d->multiplicacion = (long long)d->a * (long long)d->b;
    d->listo[OP_MULT] = 1;
    printf("[MULTIPLICACION] PID=%d escribi mi resultado en mi campo de la memoria compartida\n", (int)getpid());

    sleep(ESPERA);   /* para dar tiempo de monitorear */
    shmdt(d);
    printf("[MULTIPLICACION] PID=%d terminando\n", (int)getpid());
    return 0;
}
