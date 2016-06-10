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

void error(const char *msg)
{
    perror(msg);
    exit(0);
}


void * get_in_addr(struct sockaddr * sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}


void stream_srv(int sockfd)
{
    char buf[MAXLINE];
    int bytes_read, bytes_write;
    
    while(1) {
        memset(buf, 0, sizeof buf);
        bytes_read = read(sockfd, buf, sizeof buf);
        if ( bytes_read < 0 ) error("[srv] read");
        
        if ( strcmp(buf, "bye") == 0 ) {
            close(sockfd);
            break;
        }
        

        FILE *file = fopen(buf, "rb");
        if ( file == NULL ) error("[srv] fopen");
        printf("\ntcpsrv: file %s opened success...\n", buf);
        
        memset(buf, 0, sizeof buf);
        while ( fgets(buf, MAXLINE-1, file) != NULL ) {
            bytes_write = write(sockfd, buf, sizeof buf);
            if ( bytes_write < 0 ) error("[srv] write");
            printf("\t%d bytes transfered\n", bytes_write);
            memset(buf, 0, sizeof buf);
        }
        fclose(file);
    }
}


int main(int argc, char * argv[])
{
    signal(SIGCHLD, SIG_IGN);
    struct addrinfo hints, *res, *p;
    int status, sockfd, newsockfd, pid;
    int optval = 1;
    const char *srvport;
    struct sockaddr_storage cli_addr;
    socklen_t cli_addrlen;
    char ipstr[INET6_ADDRSTRLEN];

    
    if (argc == 1) {
        srvport = PORT;
    } else if (argc == 3) {
        srvport = argv[2];
    } else {
        fprintf(stderr, "\ntcpsrv: illegal option\nUsage: tcpsrv [-p port]");
        exit(1);
    }
    
    
    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    
    if ( (status = getaddrinfo(NULL, srvport, &hints, &res)) != 0) {
        fprintf(stderr, "[srv] getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    
    for (p = res; p != NULL; p = p->ai_next) {
        if ( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 ) {
            perror("[srv] socket");
            continue;
        }

        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

        if ( bind(sockfd, p->ai_addr, p->ai_addrlen) == -1 ) {
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
    printf("\ntcpsrv: socket %d waiting for data on port TCP %s\n", sockfd, srvport);

    
    if ( listen(sockfd, BACKLOG) == -1 ) error("[srv] listen");

    
    cli_addrlen = sizeof cli_addr;
    while(1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_addrlen);
        if ( newsockfd == -1 ) error("[srv] accept");
        
        printf("\ntcpsrv: accept new socket %d from client %s\n",
               newsockfd,
               inet_ntop(cli_addr.ss_family,
                          get_in_addr((struct sockaddr *)&cli_addr),
                         ipstr, sizeof ipstr));
        
        pid = fork();
        if ( pid == -1 ) error("[srv] fork");
        if ( pid == 0 ) {
            close(sockfd);
            stream_srv(newsockfd);
            exit(0);
        } else close(newsockfd);
    }
    
    close(sockfd);
    return 0;
}