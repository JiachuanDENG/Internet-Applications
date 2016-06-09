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

#define PORT "6280"
#define MAXBUFLEN 100

void * get_in_addr(struct sockaddr * sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
    struct addrinfo hints, * res, * p;
    int status, sockfd;
    char ipstr[INET6_ADDRSTRLEN], buf[MAXBUFLEN];
    struct sockaddr_storage cli_addr;
    socklen_t cli_addrlen;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ( (status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if ( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("[srv] socket");
            continue;
        }

        if ( bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
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

    printf("\n[srv] waiting to recvfrom...\n");

    cli_addrlen = sizeof cli_addr;
    if ( (numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1, 0, (struct sockaddr *)&cli_addr, &cli_addrlen)) == -1) {
        perror("[srv] recvfrom");
        exit(1);
    }

    printf("[srv] got packet from %s\n",
        inet_ntop(cli_addr.ss_family,
            get_in_addr((struct sockaddr *)&cli_addr),
            ipstr, sizeof ipstr));

    printf("[srv] packet size: %d bytes\n", numbytes);
    buf[numbytes] = '\0';
    printf("[srv] packet now contains %s\n", buf);

    close(sockfd);

    return 0;
}
       

