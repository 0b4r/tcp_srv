#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 3434
#define BACKLOG 3
#define MAX_RES 256

int main(void)
{
    int server_fd;
    int opt = 1;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, addrlen) < 0) {
        perror("binding failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    int client_fd;
    struct sockaddr_in client_address;
    socklen_t client_addrlen = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_addrlen);

    if (client_fd < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    char msg[MAX_RES] = {0};

    ssize_t bytes_recv = recv(client_fd, msg, sizeof(msg) - 1, 0);
    if (bytes_recv < 0) {
        perror("recv failed");
    } else if (bytes_recv == 0) {
        printf("client disconnect\n");
    } else {

        msg[bytes_recv] = '\0';
        msg[strcspn(msg, "\r\n")] = '\0';

        char *rep = "pong\r\n";
        if (strcasecmp(msg, "PING") == 0) {
            ssize_t bytes_sent = send(client_fd, rep, strlen(rep), 0); 
            if (bytes_sent < 0) {
                perror("send");
            }
        }
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
