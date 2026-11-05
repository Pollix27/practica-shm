#include "comun.h"

/* Proceso hijo: resta. Recibe por argumento el shmid de SU memoria compartida. */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <shmid>\n", argv[0]);
        return 1;
    }
    int shmid = atoi(argv[1]);

    Mem *m = (Mem *)shmat(shmid, NULL, 0);
    if (m == (void *)-1) { perror("shmat"); return 1; }

    printf("[RESTA] PID=%d PPID=%d shmid=%d -> leo a=%d b=%d\n",
           (int)getpid(), (int)getppid(), shmid, m->a, m->b);

    m->res_entero = (long long)m->a - (long long)m->b;
    m->listo = 1;
    printf("[RESTA] PID=%d escribi mi resultado en la memoria compartida\n", (int)getpid());

    sleep(ESPERA);   /* para dar tiempo de monitorear */
    shmdt(m);
    printf("[RESTA] PID=%d terminando\n", (int)getpid());
    return 0;
}
