#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

int fd, pipe_fd;

void sig_handler(int signum, siginfo_t *info, void *context);

int main(int argc, char* argv[]) {

    // < -- log info -- >
    printf("P1: executed\n");

    // < -- argument correctness check -- >
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fd rúry r1 na zápis>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 1. parse argument of pipe
    pipe_fd = atoi(argv[1]);

    // 2. open file to read from
    fd = open("p1.txt", O_RDONLY);
    if (fd == -1) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    // 3. receive all signals to this process, to function sig_handler()
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_flags = SA_SIGINFO;
    sig.sa_sigaction = sig_handler;
    sigaction(SIGUSR1, &sig, NULL);

    // 4. send signal to the parent, that this process is prepared.
    kill(getppid(), SIGUSR1);

    while(1) {
        pause();
    }

    return 0;
}


void sig_handler(int signum, siginfo_t *info, void *context) {

    // 1. signal received

    // 2. determine the sender
    pid_t sender_pid = info -> si_pid;
    printf("P1: -> Receive signal [%d] from  [%d]\n", signum, sender_pid);

    // 3. initialize data to read 1 word from p1.txt
    char input[151];
    memset(&input, 0, sizeof(input));
    char buffer;
    off_t pos = 0;

    // 4. read no more then 150 symbols, and last symbol will be \0
    while (pos < 150) {

    ssize_t r = read(fd, &buffer, 1);
    if (r > 0) {

    input[pos++] = buffer;
    if (buffer == '\n') {
    break;
    }

    }

    else {
    if (pos == 0) return;
    break;
    }

    }

    if (pos > 0) {
    input[pos] = '\0';
    printf("P1: Writing -- \'%s\' --\n", input);
    if (write(pipe_fd, &input, pos) == -1) {
        perror("write()");
        exit(EXIT_FAILURE);
    }
    }
}
