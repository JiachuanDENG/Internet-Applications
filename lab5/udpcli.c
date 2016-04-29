//
//  udpcli.c
//
//
//  Created by Victoria Mengqi LIU on 4/8/16.
//
//

#include "../lib/ial.h"
#include "../lib/wrap.c"
#include "../lib/datagram.c"
#include "../lib/sock_ntop.c"

int
main(int argc, char **argv)
{
    int					sockfd;
    socklen_t			len;
    struct sockaddr_in	cliaddr, servaddr;
    
    if (argc != 2)
    {
        printf("usage: udpcli <IPaddress>");
        exit(1);
    }
    
    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
    
    len = sizeof(cliaddr);
    Getsockname(sockfd, (SA *) &cliaddr, &len);
    printf("local address %s\n", Sock_ntop((SA *) &cliaddr, len));
    
    exit(0);
}
