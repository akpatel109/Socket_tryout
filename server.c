#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_CONNECTIONS 4
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[BUFFER_SIZE];

    if (argc < 3) {
        fprintf(stderr,"ERROR, usage %s <port> <backup_host> <backup_port>\n", argv[0]);
        exit(1);
    }

    // Create a socket for the server
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // Set up the server address
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // Listen for incoming connections
    listen(sockfd, 5);

    // Accept incoming connections and handle them
    clilen = sizeof(cli_addr);
    int connection_count = 0;
    while (1) {
        if (connection_count < MAX_CONNECTIONS) {
            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
            if (newsockfd < 0) {
                perror("ERROR on accept");
                exit(1);
            }

            // Increment the connection count
            connection_count++;
            printf("connection count is %d \n",connection_count);

            // Fork a child process to handle the connection
            pid_t pid = fork();
            if (pid < 0) {
                perror("ERROR on fork");
                exit(1);
            }

            if (pid == 0) {  // child process
                // Close the listening socket
                close(sockfd);

                // Read the file name from the client
                bzero(buffer, BUFFER_SIZE);
                n = read(newsockfd, buffer, BUFFER_SIZE-1);
                if (n < 0) {
                    perror("ERROR reading from socket");
                    exit(1);
                }

                printf("command from buffer %s\n",buffer);

                // Open the file for reading
                FILE *file = fopen(buffer, "rb");
                if (file == NULL) {
                    perror("ERROR opening file");
                    exit(1);
                }

                // Send the file to the client
                bzero(buffer, BUFFER_SIZE);
                int bytes_read = 0;
                while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
                    n = write(newsockfd, buffer, bytes_read);
                    if (n < 0) {
                        perror("ERROR writing to socket");
                        exit(1);
                    }
                    bzero(buffer, BUFFER_SIZE);
                }

                // Close the file and socket
                fclose(file);
                close(newsockfd);

                // Decrement the connection count
                connection_count--;
                exit(0);
            }
            else {  // parent process
                //
    close(newsockfd);
                }}}
    while(waitpid(-1, NULL, WNOHANG) > 0); // clear finished child processes

    // // Check if maximum connections are reached
    // if (client_count >= MAX_CONNECTIONS) {
    //     printf("Maximum connections reached. Redirecting new connections to backup server...\n");
    //     close(sockfd);

    //     // Create socket to connect to backup server
    //     struct sockaddr_in backup_addr;
    //     bzero((char *) &backup_addr, sizeof(backup_addr));
    //     int backup_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //     if (backup_sockfd < 0) {
    //         perror("Error opening socket");
    //         exit(1);
    //     }

    //     // Set up the address of the backup server
    //     backup_addr.sin_family = AF_INET;
    //     backup_addr.sin_port = htons(BACKUP_PORT);
    //     if (inet_pton(AF_INET, BACKUP_IP, &backup_addr.sin_addr) <= 0) {
    //         perror("Invalid address or address not supported");
    //         exit(1);
    //     }

    //     // Connect to the backup server
    //     if (connect(backup_sockfd, (struct sockaddr*) &backup_addr, sizeof(backup_addr)) < 0) {
    //         perror("Error connecting to backup server");
    //         exit(1);
    //     }

    //     // Serve clients from the backup server
    //     printf("Serving clients from backup server...\n");
    //     while (1) {
    //         socklen_t clilen = sizeof(cli_addr);
    //         newsockfd = accept(backup_sockfd, (struct sockaddr*) &cli_addr, &clilen);
    //         if (newsockfd < 0) {
    //             perror("Error on accept");
    //             exit(1);
    //         }
    //         client_count++;
    //         printf("Client %d connected from %s:%d\n", client_count, inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
    //         pid_t pid = fork();
    //         if (pid < 0) {
    //             perror("Error on fork");
    //             exit(1);
    //         }
    //         if (pid == 0) {  // child process
    //             close(sockfd);
    //             handle_request(newsockfd, client_count);
    //             exit(0);
    //         }
    //         else {  // parent process
    //             close(newsockfd);
    //         }
    //     }
    // }
    // else { // Serve the client request
    //     client_count++;
    //     printf("Client %d connected from %s:%d\n", client_count, inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
    //     pid_t pid = fork();
    //     if (pid < 0) {
    //         perror("Error on fork");
    //         exit(1);
    //     }
    //     if (pid == 0) {  // child process
    //         close(sockfd);
    //         handle_request(newsockfd, client_count);
    //         exit(0);
    //     }
    //     else {  // parent process
    //         close(newsockfd);
    //     }
    // }
}
