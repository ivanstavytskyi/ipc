#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char *shm_addr;

void cleanup(int sig) {
  shmdt(shm_addr);
  exit(0);
}

void sem_v(int sem_id) {
    struct sembuf sem_op = {0, 1, 0};
    if (semop(sem_id, &sem_op, 1) == -1) {
        perror("semop()");
        exit(EXIT_FAILURE);
    }
}

void sem_p(int sem_id) {
    struct sembuf sem_op = {1, -1, 0};
    if (semop(sem_id, &sem_op, 1) == -1) {
        perror("semop()");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <id zdieľanej pamäte SM2> <id semaforu S2> <číslo portu 1> ", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGTERM, cleanup);

    int shm_sm2, sem2, port1;

    shm_sm2 = atoi(argv[1]);
    sem2 = atoi(argv[2]);
    port1 = atoi(argv[3]);
    printf("D: Port %d\n", port1);

    char *shm_addr = (char*)shmat(shm_sm2, NULL, 0);
    if (shm_addr == (void*)-1) {
        perror("shmat()");
        exit(EXIT_FAILURE);
    }

    kill(getppid(), SIGUSR1);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port1);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    while (1) {
    sem_p(sem2);
    usleep(100000);

    printf("\n\nD: SM2 data: --%s--\n\n", shm_addr);

    if (strlen(shm_addr) == 0) {
        sem_v(sem2);
        break;
    }

    printf("\n\nD: input to the socket: --%s-- (sending 150 bytes)\n\n", shm_addr);

    if (write(sock, shm_addr, 151) == -1) {
        perror("write()");
        exit(EXIT_FAILURE);
    }

    sem_v(sem2);
    usleep(100000);
    }

    close(sock);


    shmdt(shm_addr);

    return 0;
}
