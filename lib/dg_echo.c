//
// dg_echo.c        /* Read datagram, echo back to sender */
//
//
// Created by Victoria Mengqi LIU on  4/7/16
//
//

#include "ial.h"
#include "wrap.c"

/* dg_echo function: echo lines on a datagrame socket. */
void
dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    int n;
    socklen_t len;
    char mesg[MAXLINE];
    
    /* reads the next datagram arriving at the server's port */
    for ( ; ; )
    {
        len = clilen;
        n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
        Sendto(sockfg, mesg, n, 0, pcliaddr, len);
    }
}