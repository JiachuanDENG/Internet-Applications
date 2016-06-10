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


void error(const char *msg)
{
    perror(msg);
    exit(0);
}


void stream_cli(int sockfd)
{
    char buf[MAXLINE], tmp[MAXLINE];
    int bytes_read, bytes_write;
    
    memset(buf, 0, sizeof buf);
    fgets(buf, MAXLINE-1, stdin);
    strtok(buf, "\n");
    if ( strcmp(buf, "bye") == 0 ) {
        close(sockfd);
        exit(0);
    }
    
    printf("\nSave as: ");
    fgets(tmp, MAXLINE-1, stdin);
    strtok(tmp, "\n");
    FILE *file = fopen(tmp, "ab");
    memset(tmp, 0, sizeof tmp);
    
    if ( write(sockfd, buf, sizeof buf) < 0 ) error("[cli] write");
    memset(buf, 0, sizeof buf);
    
    while ( (bytes_read = read(sockfd, buf, sizeof buf)) > 1 ) {
        fputs(buf, file);
    }
    fclose(file);
    printf("\nTRANSFER FINISHED.");
    memset(buf, 0, sizeof buf);
}

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
    int status, sockfd;
    const char *srvport;
    char ipstr[INET6_ADDRSTRLEN];

    
    if ( strcmp(argv[1], "-h") == 0 ) {
        if ( argc == 3 ) {
            srvport = PORT;
        } else if (argc == 5) {
            srvport = argv[4];
        }
    } else {
        fprintf(stderr, "\ntcpcli: illegal option\nUsage: tcpcli -h host [-p port]");
        exit(1);
    }

    
    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    
    if ( (status = getaddrinfo(argv[2], PORT, &hints, &res)) != 0 ) {
        fprintf(stderr, "[cli] getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    
    for (p = res; p != NULL; p = p->ai_next) {
        if ( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 ) {
            perror("[cli] socket");
            continue;
        }
        
        if ( connect(sockfd, p->ai_addr, p->ai_addrlen) == -1 ) {
            close(sockfd);
            perror("[cli] connect");
            continue;
        }
        
        break;
    }

    
    freeaddrinfo(res);
    if (p == NULL) {
        fprintf(stderr, "[cli] failed to create socket");
        exit(1);
    }
    printf("\ntcpcli: [socket %d] connect to server %s\n",
           sockfd,
           inet_ntop(p->ai_family,
                     get_in_addr((struct sockaddr *)&(p->ai_addr)),
                     ipstr, sizeof ipstr));
    
    
    while(1) {
        printf("\nFile to download: ");
        stream_cli(sockfd);
    }
        
    close(sockfd);
    return 0;
}