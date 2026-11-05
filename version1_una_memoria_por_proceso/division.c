#include "comun.h"

/* Proceso hijo: division. Recibe por argumento el shmid de SU memoria compartida. */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <shmid>\n", argv[0]);
        return 1;
    }
    int shmid = atoi(argv[1]);

    Mem *m = (Mem *)shmat(shmid, NULL, 0);
    if (m == (void *)-1) { perror("shmat"); return 1; }

    printf("[DIVISION] PID=%d PPID=%d shmid=%d -> leo a=%d b=%d\n",
           (int)getpid(), (int)getppid(), shmid, m->a, m->b);

    if (m->b == 0) m->error = 1;
    else            m->res_real = (double)m->a / (double)m->b;
    m->listo = 1;
    printf("[DIVISION] PID=%d escribi mi resultado en la memoria compartida\n", (int)getpid());

    sleep(ESPERA);   /* para dar tiempo de monitorear */
    shmdt(m);
    printf("[DIVISION] PID=%d terminando\n", (int)getpid());
    return 0;
}
