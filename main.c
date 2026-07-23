#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define OUT_PORT 2324
#define MSG_MAXLEN 1024

int main(void)
{
    int srv_fd;
    int yes = 1;
    struct sockaddr_in srv_addr = {0};
    socklen_t srv_addr_len = sizeof(srv_addr);
    
    srv_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (srv_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(srv_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(OUT_PORT);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(srv_fd, (struct sockaddr *)&srv_addr, srv_addr_len) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    if (listen(srv_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("listening ..\n");
    
    int usr_fd;
    struct sockaddr_in usr_addr = {0};
    socklen_t usr_addr_len = sizeof(usr_addr);
    char usr_ip[INET_ADDRSTRLEN] = {0};
    while (1) {
        usr_fd = accept(srv_fd, (struct sockaddr *)&usr_addr, &usr_addr_len);
        
        if (usr_fd < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        inet_ntop(AF_INET, &usr_addr.sin_addr, usr_ip, sizeof(usr_ip));
        
        printf("received connection from %s:%d\n", usr_ip, ntohs(usr_addr.sin_port));
        
        char usr_msg[MSG_MAXLEN] = {0};
        while (1) {
            ssize_t msg_len = recv(usr_fd, usr_msg, sizeof(usr_msg), 0);
            
            if (msg_len == 0) {
                printf("client %s:%d disconnected\n", usr_ip, ntohs(usr_addr.sin_port));
                break;
            } else if (msg_len < 0) {
                perror("recv");
            } else {
                
            }
        }
    }
    close(usr_fd);
    close(srv_fd);
    
    return 0;
}    