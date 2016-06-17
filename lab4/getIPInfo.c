//
// Description: show IP for CLI type-in host name
// 
// Author: Victoria Mengqi LIU
//
// Date: 5/5/16
//
// Version: 1.0.1

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SA4 struct sockaddr_in
#define SA6 struct sockaddr_in6


int main(int argc, char *argv[]) {
	
	struct addrinfo hints, *res, *p;
	int status;
	char ipstr[INET6_ADDRSTRLEN];

	if (argc != 2) {
		fprintf(stderr, "usage: getIPInfo <hostname>\n");
		return 1;
	}

	// initialize
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	// getaddrinfo
	if ( (status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 2;
	}

	printf("IP address for %s:\n\n", argv[1]);

	// get IP version
	for (p = res; p != NULL; p = p->ai_next) {
		void *addr;
		char *ipver;

		// IPv4
		if (p->ai_family == AF_INET) {
			SA4 *ipv4 = (SA4 *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		}
		// IPv6
		else {
			SA6 *ipv6 = (SA6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		printf("\t%s: %s\n", ipver, ipstr);
	}

	freeaddrinfo(res);

	return 0;
}

	



