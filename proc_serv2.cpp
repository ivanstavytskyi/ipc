#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <číslo portu 2>", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port2 = atoi(argv[1]);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port2);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    kill(getppid(), SIGUSR1);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen("serv2.txt", "a");
    if (file == NULL) {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }

    char buffer[151];
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);

    while (1) {
        ssize_t len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&sender, &sender_len);
        if (len > 0) {
            buffer[len] = '\0';
            fprintf(file, "%s\n", buffer);
            fflush(file);
        } else {
            break;
        }
    }

    fclose(file);
    close(sock);

    return 0;
}
