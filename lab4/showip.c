//
// Author: Victoria Mengqi LIU
//
// Date: 26/05/16
//

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT "6280"

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
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr, "usage: showip <hostname> \n");
	return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_CANONNAME;

    if ( (status = getaddrinfo(argv[1], argv[2], &hints, &res) != 0)) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
	return 2;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        
	inet_ntop(p->ai_family , get_in_addr(p->ai_addr), ipstr, sizeof ipstr);

        if (p->ai_family == AF_INET) {
            printf("\n\tIPv4 Address: %s\n", ipstr);
        }
        else {
            printf("\n\tIPv6 Address: %s\n", ipstr);
        }
    
        printf("\tCanonname: %s\n\n", p->ai_canonname);

    }

    freeaddrinfo(res);

    return 0;
}
