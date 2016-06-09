//
// Author: Victoria Mengqi LIU
//
// Date: 29/05/16
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

#define PORT "8260"
#define MAXLINE 100

void dgram_cli(FILE * fp, int sockfd, const struct sockaddr * to, socklen_t tolen)
{
    int n, numbytes;
    char sendline[MAXLINE], recvline[MAXLINE+1];

    while ( fgets(sendline, MAXLINE, fp) != 0) {
        if ( (n = sendto(sockfd, sendline, sizeof sendline, 0, to, tolen)) == -1) {
            perror("[cli] sendto");
            exit(1);
        }
        
        if ( (numbytes = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL)) == -1) {
            perror("[cli] recvfrom");
            exit(1);
        }
        recvline[numbytes] = '\0';
        
        fputs(recvline, stdout);
    }
}

int main(int argc, char * argv[])
{
    struct addrinfo hints, * res, * p;
    int status, sockfd;
    const char * srvport;
    
    if (argc < 2) {
        fprintf(stderr, "[cli] usage: udpcli <server IP>\n");
        exit(1);
    } else if (argc == 2) {
        srvport = PORT;
    } else {
        srvport = argv[2];
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ( (status = getaddrinfo(argv[1], srvport, &hints, &res)) != 0) {
        fprintf(stderr, "[cli] getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if ( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("[cli] socket");
            continue;
        }
        
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "[cli] failed to create socket");
        return 2;
    }
    
    dgram_cli(stdin, sockfd, p->ai_addr, p->ai_addrlen);
    
    freeaddrinfo(res);
    
    close(sockfd);
    return 0;
}

