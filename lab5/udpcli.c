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

#define SRVPORT "6280"

int main(int argc, char * argv[])
{
    struct addrinfo hints, * res, * p;
    int status, sockfd;
    int numbytes;

    if (argc != 3) {
        fprintf(stderr, "[cli] usage: udpcli <server IP> <message>\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ( (status = getaddrinfo(argv[1], SRVPORT, &hints, &res)) != 0) {
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

    if ( (numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0, p->ai_addr, p->ai_addrlen)) == -1) {
        perror("[cli] sendto");
        exit(1);
    }

    freeaddrinfo(res);

    printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
    close(sockfd);

    return 0;
}

