#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>

#define BUFFER_SIZE 1024

volatile sig_atomic_t keepRunning = 1;

void intHandler(int sig) {
    keepRunning = 0;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, intHandler);

    if (argc != 4) {
        printf("Usage: %s <ip> <port> <max_clients>\n", argv[0]);
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int maxClients = atoi(argv[3]);

    // Создание сокета
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        perror("Failed to create socket");
        return 1;
    }

    // Настраиваем сервер
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    // Связываем сокет с адресом сервера
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Failed to bind");
        return 1;
    }

    printf("Server started. Listening on %s:%d\n", ip, port);

    while (keepRunning) {
        char buffer[BUFFER_SIZE];
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        memset(buffer, 0, BUFFER_SIZE);

        // Получаем запрос от клиента
        int bytesRead = recvfrom(serverSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (bytesRead < 0) {
            perror("Failed to receive data from client");
            continue;
        }

        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIp, INET_ADDRSTRLEN);
        printf("Received request from client %s:%d: %s\n", clientIp, ntohs(clientAddr.sin_port), buffer);

        if (strcmp(buffer, "хочу поесть") == 0) {
            sleep(3);
            strcpy(buffer, "можешь есть");
        } else {
            strcpy(buffer, "подожди, горшок пустой");
        }

        // Отправляем ответ клиенту
        if (sendto(serverSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&clientAddr, clientAddrLen) < 0) {
            perror("Failed to send response to client");
            continue;
        }
    }

    // Закрываем сокет сервера
    close(serverSocket);

    return 0;
}
