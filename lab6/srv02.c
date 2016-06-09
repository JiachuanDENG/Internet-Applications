//
// TCP SERVER
//
// Author: Victoria Mengqi LIU
//
// Date: 06/06/16
//

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT "8260"
#define MAXLINE 10
#define BACKLOG 10

void * get_in_addr(struct sockaddr * sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char * argv[])
{
    struct addrinfo hints, *res, *p;
    int status, sockfd, acceptfd;
    int bytes_recv, bytes_sent;
    int yes = 1;
    const char *srvport;
    struct sockaddr_storage cli_addr;
    socklen_t cli_addrlen;
    char ipstr[INET6_ADDRSTRLEN], recvline[MAXLINE];

    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (argc == 1) {
        srvport = PORT;
    } else if (argc == 3) {
        srvport = argv[2];
    } else {
        fprintf(stderr, "\ntcpsrv: illegal option\nUsage: tcpsrv [-p port]");
        exit(1);
    }
    
    if ( (status = getaddrinfo(NULL, srvport, &hints, &res)) != 0) {
        fprintf(stderr, "[srv] getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        // socket
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if ( sockfd == -1 ) {
            perror("[srv] socket");
            continue;
        }

        // setsockopt
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        // bind
        if ( bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("[srv] bind");
            continue;
        }

        break;
    }

    freeaddrinfo(res);
    
    if (p == NULL) {
        fprintf(stderr, "[srv] failed to bind socket\n");
        exit(1);
    }

    // listen
    if ( listen(sockfd, BACKLOG) == -1 ) {
        perror("[srv] listen");
        exit(1);
    }

    printf("\ntcpsrv: waiting for data on port TCP %s\n", srvport);
    
     while(1) {
         cli_addrlen = sizeof cli_addr;
         acceptfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_addrlen);
         if ( acceptfd == -1 ) {
             perror("[srv] accept");
             exit(1);
         }
         
         printf("\ntcpsrv: from %s socket %d : \n",
                inet_ntop(cli_addr.ss_family,
                          get_in_addr((struct sockaddr *)&cli_addr),
                          ipstr, sizeof ipstr),
                acceptfd);
         

        bytes_recv = recv(acceptfd, recvline, MAXLINE-1, 0);
        if ( bytes_recv == -1 ) {
            perror("[srv] recv");
            exit(1);
        }
        recvline[bytes_recv] = '\0';

        printf("tcpsrv: recv %d bytes from socket %d : %s\n", bytes_recv, acceptfd, recvline);

        if ( send(acceptfd, "TEST SUCCESS", 12, 0) == -1 ) {
            perror("[srv] send");
            exit(1);
        }
    }
    
    close(sockfd);

    return 0;
}
       

