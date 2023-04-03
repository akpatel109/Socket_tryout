#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1" // Change to the IP address of the server
#define SERVER_PORT 8080 // Change to the port number used by the server

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(SERVER_PORT);

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("connect() error");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server.\n");

    while (1) {
        char command[256];
        printf("Enter command: ");
        fgets(command, sizeof(command), stdin);
        strtok(command, "\n"); // Remove trailing newline

        // Send command to the server
        if (send(sock, command, strlen(command), 0) == -1) {
            perror("send() error");
            exit(EXIT_FAILURE);
        }

        char response[1024];
        memset(response, 0, sizeof(response));

        // Receive response from the server
        if (recv(sock, response, sizeof(response), 0) == -1) {
            perror("recv() error");
            exit(EXIT_FAILURE);
        }

        printf("Server response: %s\n", response);
    }

    close(sock);

    return 0;
}
