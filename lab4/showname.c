//
// Author: Victoria0628
//
// Date: 29/05/16
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char * argv[])
{
    struct addrinfo hints, * res, * p;
    int status;
    char host[NI_MAXHOST], serv[NI_MAXSERV];

    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE | AI_CANONNAME;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ( argc != 2) {
        fprintf(stderr, "usage: showname <ip>\n");
        return 1;
    }

    if ( (status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if ( (status = getnameinfo(p->ai_addr, p->ai_addrlen, host, sizeof host, NULL, 0, 0)) != 0) {
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(status));
            return 3;
        }

        printf("\n\tHost: %s\n", host);
        printf("\tCanonname: %s\n\n", p->ai_canonname);
    }

    freeaddrinfo(res);

    return 0;
}
