#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/signal.h>
#include <memory.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
volatile sig_atomic_t ready_count = 0;

int proc_p1, proc_p2, proc_t, proc_s, proc_d, proc_serv1, proc_serv2, proc_pr;

void sigHandler(int signum, siginfo_t *info, void *context)
{
    pid_t sender_pid = info -> si_pid;
    printf("Zadanie: prijal signal [%d] od pid - [%d]\n", signum, sender_pid);
    ready_count++;
};

char *toString(int num)
{
    char *string = (char *)malloc(10 * sizeof(char));
    memset(string, '\0', 10);
    sprintf(string, "%d", num);
    return string;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Spustenie : zadanie <číslo portu 1> <číslo portu 2>");
        exit(EXIT_FAILURE);
    }

    int serv_tcp_port_1 = atoi(argv[1]), serv_udp_port_2 = atoi(argv[2]);
    int pipe_r1[2], pipe_r2[2];
    if (pipe(pipe_r1) == -1 || pipe(pipe_r2) == -1)
    {
        perror("pipe()");
        exit(EXIT_FAILURE);
    }

    int shmid_sm1, shmid_sm2;
    shmid_sm1 = shmget(IPC_PRIVATE, 151, 0666 | IPC_CREAT);
    if (shmid_sm1 == -1)
    {
        perror("shmget()");
        exit(EXIT_FAILURE);
    }

    shmid_sm2 = shmget(IPC_PRIVATE, 151, 0666 | IPC_CREAT);
    if (shmid_sm2 == -1)
    {
        perror("shmid()");
        exit(EXIT_FAILURE);
    }

    int sem_s1, sem_s2;
    sem_s1 = semget(IPC_PRIVATE, 2, 0666 | IPC_CREAT);
    sem_s2 = semget(IPC_PRIVATE, 2, 0666 | IPC_CREAT);
    if (sem_s1 == -1 || sem_s2 == -1)
    {
        perror("semget()");
        exit(EXIT_FAILURE);
    }

    semctl(sem_s1, 0, SETVAL, 1);
    semctl(sem_s1, 1, SETVAL, 0);
    semctl(sem_s2, 0, SETVAL, 1);
    semctl(sem_s2, 1, SETVAL, 0);


    // ===========================
    // SIGNAL RECEIVING
    // ===========================

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sigHandler;
    sigaction(SIGUSR1, &sa, NULL);


    /* ==== Spustenie procesov ==== */

    /* == proc_p1 == */
    proc_p1 = fork();
    if (proc_p1 == 0)
    {
        if (execl("proc_p1",
                  "proc_p1",
                  toString(pipe_r1[1]),
                  (char *)NULL) == -1)
        {
            perror("execl()");
            exit(1);
        }
    }
    else if (proc_p1 == -1)
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    } else {
        printf("\n\n<<- P1 forked: pid[%d] ->>\n\n", proc_p1);
        pause();
        sleep(5);
    }

    /* == proc_p2 == */
    proc_p2 = fork();
    if (proc_p2 == 0)
    {
        if (execl("proc_p2",
                  "proc_p2",
                  toString(pipe_r1[1]),
                  (char *)NULL) == -1)
        {
            perror("execl()");
            exit(1);
        }
    }
    else if (proc_p2 == -1)
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    } else {
        printf("\n\n<<<-P2 forked: pid[%d]->>\n\n<", proc_p2);
        pause();
        sleep(5);
    }

    /* == proc_pr == */
    proc_pr = fork();
    if (proc_pr == 0)
    {
        if (execl("proc_pr",
                  "proc_pr",
                  toString(proc_p1),
                  toString(proc_p2),
                  toString(pipe_r1[0]),
                  toString(pipe_r2[1]),
                  (char *)NULL) == -1)
        {
            perror("execl()");
            exit(1);
        }
    }
    else if (proc_pr == -1)
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    } else {
        printf("\n\n<<<-Pr forked: pid[%d]->>\n\n<", proc_pr);
        pause();
        sleep(5);
    }

    /* == proc_t == */
    proc_t = fork();
    if (proc_t == 0)
    {
        if (execl("proc_t",
                  "proc_t",
                  toString(pipe_r2[0]),
                  toString(shmid_sm1),
                  toString(sem_s1),
                  (char *)NULL) == -1)
        {
            perror("execl()");
            exit(1);
        }
    }
    else if (proc_t == -1)
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    } else {
        printf("\n\n<<<-T forked: pid[%d]->>\n\n", proc_t);
        pause();
        sleep(5);
    }

    /* == proc_s == */
    proc_s = fork();
    if (proc_s == 0)
    {
        if (execl("proc_s",
                  "proc_s",
                  toString(shmid_sm1),
                  toString(sem_s1),
                  toString(shmid_sm2),
                  toString(sem_s2),
                  (char *)NULL) == -1)
        {
            perror("execl()");
            exit(1);
        }
    }
    else if (proc_s == -1)
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    } else {
        printf("\n\n<<<-S forked: pid[%d]->>\n\n<", proc_s);
        pause();
        sleep(5);
    }

    /* == proc_serv1 == */
    proc_serv1 = fork();
    if (proc_serv1 == 0)
    {
        if (execl("proc_serv1",
                  "proc_serv1",
                  toString(serv_tcp_port_1),
                  toString(serv_udp_port_2),
                  (char *)NULL) == -1)
        {
            perror("execl()");
            exit(1);
        }
    }
    else if (proc_serv1 == -1)
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    } else {
        printf("\n\n<<<-Serv1 forked: pid[%d]->>\n\n<", proc_serv1);
        pause();
        sleep(5);
    }

    /* == proc_serv2 == */
    proc_serv2 = fork();
    if (proc_serv2 == 0)
    {
        if (execl("proc_serv2",
                  "proc_serv2",
                  toString(serv_udp_port_2),
                  (char *)NULL) == -1)
        {
            perror("execl()");
            exit(1);
        }
    }
    else if (proc_serv2 == -1)
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    } else {
        printf("\n\n<<<-Serv2 forked: pid[%d]->>\n\n<", proc_serv2);
        pause();
        sleep(5);
    }

    /* == proc_d == */
    proc_d = fork();
    if (proc_d == 0)
    {
        if (execl("proc_d",
                  "proc_d",
                  toString(shmid_sm2),
                  toString(sem_s2),
                  toString(serv_tcp_port_1),
                  (char *)NULL) == -1)
        {
            perror("execl()");
            exit(1);
        }
    }
    else if (proc_d == -1)
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    } else {
        printf("\n\n<<<-D forked: pid[%d]->>\n\n<", proc_d);
        pause();
        sleep(5);
    }

    while (ready_count < 8)
    {
        pause();
        sleep(5);
    }

    printf("arleady here, capturing all signals ---!---\n");

    close(pipe_r1[0]);
    close(pipe_r1[1]);
    close(pipe_r2[0]);
    close(pipe_r2[1]);
    kill(proc_p1, SIGTERM);
    kill(proc_p2, SIGTERM);
    kill(proc_pr, SIGTERM);
    kill(proc_t, SIGTERM);
    kill(proc_s, SIGTERM);
    kill(proc_d, SIGTERM);
    kill(proc_serv1, SIGTERM);
    kill(proc_serv2, SIGTERM);
    int status;
    while (wait(&status) > 0)
        ;
    if (shmctl(shmid_sm1, IPC_RMID, 0) == -1)
    {
        perror("shmctl()");
        exit(EXIT_FAILURE);
    }
    if (shmctl(shmid_sm2, IPC_RMID, 0) == -1)
    {
        perror("shmctl()");
        exit(EXIT_FAILURE);
    }
    if (semctl(sem_s1, 0, IPC_RMID, 0) == -1)
    {
        perror("semctl()");
        exit(EXIT_FAILURE);
    }
    if (semctl(sem_s2, 0, IPC_RMID, 0) == -1)
    {
        perror("semctl()");
        exit(EXIT_FAILURE);
    }
    return 0;
}
