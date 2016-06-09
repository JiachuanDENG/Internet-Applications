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
#define MAXLINE 4096
#define BACKLOG 10

char readf[MAXLINE], ipstr[INET6_ADDRSTRLEN], recvline[100];
int bytes_recv, bytes_sent, readf_len, acceptfd;

void * get_in_addr(struct sockaddr * sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void deal_recv(int socket, const char *restrict dir) {
    FILE * file = fopen(dir, "r");
    if (file == NULL) {
        if ( send(socket, "\ntcpsrv: No such file\n", 22, 0) == -1 ) {
            close(socket);
            perror("[srv] send");
        }
        perror("[srv] fopen");
        exit(1);
    }
    
    // read dir
    while ( fgets(readf, MAXLINE, file) != NULL ) {
        // send file
        readf_len = sizeof readf;
        bytes_sent = send(socket, readf, readf_len, 0);
        if ( bytes_sent == -1 ) {
            close(socket);
            perror("[srv] send");
            exit(1);
        }
        
        printf("\tbytes readf: %d\n\tbytes send: %d\n", readf_len, bytes_sent);
    }
    
    fclose(file);
}

void stream_srv(int sockfd, struct sockaddr * from, socklen_t fromlen)
{
    //TEST
    printf("srv.stream_srv inside while");
    // accept
    acceptfd = accept(sockfd, from, &fromlen);
    if ( acceptfd == -1 ) {
        perror("[srv] accept");
        exit(1);
    }
    
    printf("\ntcpsrv: from %s socket %d : ",
           inet_ntop(from->sa_family,
                     get_in_addr(from),
                     ipstr, sizeof ipstr),
           acceptfd);
    
    // recv
    bytes_recv = recv(acceptfd, recvline, sizeof recvline, 0);
    if ( bytes_recv <= 0 ) {
        if ( bytes_recv == 0 ) {
            printf("\ntcpsrv: socket %d hung up\n", acceptfd);
        } else {
            perror("[srv] recv");
        }
        
        close(acceptfd);
        exit(0);
    }
    
    deal_recv(acceptfd, recvline);
}

int main(int argc, char * argv[])
{
    struct addrinfo hints, * res, * p;
    int status, sockfd;
    int yes = 1;
    const char * srvport;
    struct sockaddr_storage cli_addr;
    socklen_t cli_addrlen;

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

    if (p == NULL) {
        fprintf(stderr, "[srv] failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(res);

    // listen
    if ( listen(sockfd, BACKLOG) == -1 ) {
        perror("[srv] listen");
        exit(1);
    }

    printf("\ntcpsrv: waiting for data on port TCP %s\n", srvport);
    
    while(1) {
        cli_addrlen = sizeof cli_addr;
        stream_srv(sockfd, (struct sockaddr *)&cli_addr, cli_addrlen);
    }
    
    close(sockfd);

    return 0;
}
       

