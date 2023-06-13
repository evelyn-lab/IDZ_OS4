#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

volatile sig_atomic_t keepRunning = 1;

void intHandler(int sig) {
    keepRunning = 0;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, intHandler);
    int client_socket;
    struct sockaddr_in server_addr;
    unsigned short server_port;
    char *server_ip;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n", argv[0]);
        exit(1);
    }

    server_ip = argv[1];
    server_port = atoi(argv[2]);

    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket() failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    printf("Connected to server\n");

    char msg[32];
    int buffer[2];
    buffer[0] = 0;

    while (keepRunning) {
        socklen_t serverAddrLen = sizeof(server_addr);
        recvfrom(client_socket, msg, sizeof(msg), 0, (struct sockaddr *)&server_addr, &serverAddrLen);
        printf("[Observer] %s", msg);
        sendto(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    }

    buffer[0] = -1;
    sendto(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    close(client_socket);
    return 0;
}
