#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>

/* T process get forked by main process zadanie, then he reads the data from the pipe (R2), then he using the semafor,
when its his turn, he writes the data to virtual memory SM1, and decreasing his semaphore on time of work, and then after writing
the word to the SM1, increasing the semaphore of prebuild process S, and then waiting till its his turn, and process S,
get increase his semaphore, so he can again read word by word from pipe, and write one word at a time to virtual memory SM1. */

char* shm_addr;

void cleanup(int sig) {
  shmdt(shm_addr);
  exit(0);
}

void mutex_func(int sem_id, int sem_num, int sem_op);

int main(int argc, char *argv[]) {

    // // <-- log info -->
    printf("T: executed\n");

    // 1. check arguments correctness
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <fd rúry r2 na čítanie> <id zdieľanej pamäte SM1> <id semaforu S1>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGTERM, cleanup);

    // 2.parse arguments of pipe from where to read data, where to store read data, and semafore, to signalise when store read data.
    int pipe_r2_read = atoi(argv[1]),
    shmid_sm1 = atoi(argv[2]),
    sem_s1 = atoi(argv[3]);

    // 3. attach to shared memory
    shm_addr = (char *)shmat(shmid_sm1, NULL, 0);

    kill(getppid(), SIGUSR1);

    while (1){
    mutex_func(sem_s1, 0, -1);
    usleep(100000);
    printf("\n\nT: make my semaphore down while i'm working\n");

    // 4. read word by word from pipe r2, and store them word word by word to shared memory segment, using semaphore, to signalise,
    // that word was attached to shared memory
    off_t pos = 0;
    char buffer;
    char input[151];

    memset(&input, '\0', 151);
    memset(shm_addr, '\0', 151);

    while (pos < 150) {
    ssize_t r = read(pipe_r2_read, &buffer, 1);
    // after reading, don't place the '\n' at the end
    if (r > 0) {
        if (buffer == '\n') {
            break;
        }
        input[pos++] = buffer;
    } else {
        if (pos == 0) {
            shmdt(shm_addr);
            return 0;
        }
        break;
    }
}

    // 5. reading 1 word from pipe ended

    // 6.0 decrease current semaphore of process while writing to shared memory,
    // then after writing increase semaphore of process, S, which is going to read this one word from shared memory
    // 6.1 write saved word to the shared memory
    if (pos > 0)
    {
    input[pos] = '\0';
    // we get string in format [xxxxxxxxxx\0];
    printf("T: Receive string from pipe r2: --%s--\n", input);
    strcpy(shm_addr, input);
    printf("T: write to shared memory word, shared memory now: %s\n", shm_addr);
    printf("T: make semaphore of S up so he can read from shared memory\n\n");
    mutex_func(sem_s1, 1, 1);
    usleep(100000);
    } else {
    break;
    }

    }

    shmdt(shm_addr);

    return 0;
}


void mutex_func(int sem_id, int sem_num, int sem_op)
{
    struct sembuf sem_struct;
    sem_struct.sem_num = sem_num;
    sem_struct.sem_op = sem_op;
    sem_struct.sem_flg = 0;

    if (semop(sem_id, &sem_struct, 1) == -1)
    {
        perror("semop()");
        exit(EXIT_FAILURE);
    }
}
