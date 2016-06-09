//
// TCP CLIENT
//
// Author: Victoria Mengqi LIU
//
// Date: 06/06/16
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#define PORT "8260"
#define MAXLINE 4096

void * get_in_addr(struct sockaddr * sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char * argv[])
{
    struct addrinfo hints, * res, * p;
    int status, sockfd, bytes_recv, bytes_send;
    const char * srvport, * dir, * n_dir;
    char ipstr[INET6_ADDRSTRLEN], recvline[MAXLINE];

    
    if ( strcmp(argv[1], "-h") == 0 ) {
        if ( strcmp(argv[3], "-f") == 0 ) {
            srvport = PORT;
            dir = argv[4];
            n_dir = argv[5];
        } else if ( strcmp(argv[3], "-p") == 0 ) {
            if ( strcmp(argv[5], "-f") == 0 ) {
                srvport = argv[4];
                dir = argv[6];
                n_dir = argv[7];
            }
        }
    } else {
        fprintf(stderr, "\ntcpcli: illegal option\nUsage: tcpcli -h host [-p port] -f file");
        exit(1);
    }

    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    
    if ( (status = getaddrinfo(argv[2], PORT, &hints, &res)) != 0) {
        fprintf(stderr, "[cli] getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    
    for (p = res; p != NULL; p = p->ai_next) {
        // socket
        if ( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 ) {
            perror("[cli] socket");
            continue;
        }
        
        // connect
        if ( connect(sockfd, p->ai_addr, p->ai_addrlen) == -1 ) {
            close(sockfd);
            perror("[cli] connect");
            continue;
        }
        
        break;
    }

    
    if (p == NULL) {
        fprintf(stderr, "[cli] failed to create socket");
        return 2;
    }
    
    
    freeaddrinfo(res);
    
    
    printf("\ntcpcli: connect to server %s socket %d\n",
           inet_ntop(p->ai_family,
                     get_in_addr((struct sockaddr *)&(p->ai_addr)),
                     ipstr, sizeof ipstr),
           sockfd);
    
    printf("tcpcli: send %lu bytes to server: %s\n", strlen(dir), dir);
    if ( send(sockfd, dir, bytes_send, 0) == -1 ) {
        perror("[cli] send");
        exit(1);
    }
    
    FILE *file = fopen(n_dir, "ab");
    
    bytes_recv = recv(sockfd, recvline, MAXLINE, 0);
    while ( (bytes_recv = recv(sockfd, recvline, MAXLINE, 0)) > 1 ) {
        fputs(recvline, file);
    }
    
    fclose(file);
    close(sockfd);
    
    printf("FILE TRANSFER SUCCESS!\n");
    
    return 0;
}

