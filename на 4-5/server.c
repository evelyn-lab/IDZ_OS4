#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <ip> <port> <max_clients>\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int maxClients = atoi(argv[3]);
    // Создаем сокет
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        perror("Failed to create socket");
        return 1;
    }
    // Адрес сервера
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Failed to bind");
        return 1;
    }
    printf("Server started. Listening on %s:%d\n", ip, port);
    while (1) {
        char buffer[BUFFER_SIZE];
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        memset(buffer, 0, BUFFER_SIZE);

        // Прослушиваем входящие соединения от клиентов
        int bytesRead = recvfrom(serverSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (bytesRead < 0) {
            perror("Failed to receive data from client");
            continue;
        }

        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIp, INET_ADDRSTRLEN);
        printf("Received request from client: %s:%d - %s\n", clientIp, ntohs(clientAddr.sin_port), buffer);

        // Получаем запрос клиента
        if (strcmp(buffer, "хочу поесть") == 0) {
            sleep(3);
            strcpy(buffer, "можешь есть");
        } else {
            strcpy(buffer, "подожди, горшок пустой");
        }

        // Отправляем ответ клиенту 
        if (sendto(serverSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) < 0) {
            perror("Failed to send response to client");
            continue;
        }
        printf("Sent response to client: %s:%d - %s\n", clientIp, ntohs(clientAddr.sin_port), buffer);
    }

    // Закрываем сокет сервера
    close(serverSocket);
    return 0;
}
