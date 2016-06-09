//
// UDP SERVER
//
// Author: Victoria Mengqi LIU
//
// Date: 29/05/16
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
#define MAXLINE 100

char recvline[MAXLINE], ipstr[INET6_ADDRSTRLEN];
int numbytes;

void * get_in_addr(struct sockaddr * sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void dgram_srv(int sockfd, struct sockaddr * from, socklen_t fromlen)
{
    while(1) {
        if ( (numbytes = recvfrom(sockfd, recvline, sizeof recvline, 0, from, &fromlen)) == -1) {
            perror("[srv] recvfrom");
            exit(1);
        }
        
        printf("udpsrv: from %s : ",
               inet_ntop(from->sa_family,
                         get_in_addr(from),
                         ipstr, sizeof ipstr));
        printf("%s\n", recvline);
        
        if ( sendto(sockfd, recvline, numbytes, 0, from, fromlen) == -1) {
            perror("[srv] sendto");
            exit(1);
        }
    }
}

int main(int argc, char * argv[])
{
    struct addrinfo hints, * res, * p;
    int status, sockfd;
    const char * srvport;
    struct sockaddr_storage cli_addr;
    socklen_t cli_addrlen;

    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if (argc < 2) {
        fprintf(stderr, "[srv] usage: udpsrv <server IP> [port]\n");
        exit(1);
    } else if (argc == 2) {
        srvport = PORT;
    } else {
        srvport = argv[2];
    }
    
    if ( (status = getaddrinfo(NULL, srvport, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if ( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("[srv] socket");
            continue;
        }

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

    printf("\nudpsrv: waiting for data on port UDP %s\n", srvport);
    
    cli_addrlen = sizeof cli_addr;
    dgram_srv(sockfd, (struct sockaddr *)&cli_addr, cli_addrlen);
    
    close(sockfd);

    return 0;
}
       

