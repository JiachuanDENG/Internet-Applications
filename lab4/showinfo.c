//
// Author: Victoria Mengqi LIU
//
// Date: 29/05/16
//

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

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
    int status, err;
    char host[NI_MAXHOST], ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_CANONNAME;

    if ( (status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    printf("\naddress for %s:\n\n", argv[1]);

    for (p = res; p != NULL; p = p->ai_next) {
        if ( (err = getnameinfo(p->ai_addr, p->ai_addrlen, host, sizeof host, NULL, 0, 0)) != 0) {
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(err));
            return 2;
        }
        printf("\tHostname: %s\n", host); 
        printf("\tCanonname: %s\n", p->ai_canonname);

        inet_ntop(p->ai_family, get_in_addr(p->ai_addr), ipstr, sizeof ipstr);

        if (p->ai_family == AF_INET) {
            printf("\tIPv4 address: %s\n\n", ipstr);
        }
        else {
            printf("\tIPv6 address: %s\n\n", ipstr);
        }
    }

    freeaddrinfo(res);

    return 0;
}
