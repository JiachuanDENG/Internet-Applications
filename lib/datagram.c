//
//	datagram.c
//
//
//	Created by Victoria Mengqi LIU on  4/7/16
//
//

#include "ial.h"
//#include "wrap.c"

/* Echo lines on a datagrame socket. */
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
        Sendto(sockfd, mesg, n, 0, pcliaddr, len);
    }
}

/* Client processing loop. */
void
dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
	int n;
	char sendline[MAXLINE], recvline[MAXLINE + 1];

	while (Fgets(sendline, MAXLINE, fp) != NULL)
	{
		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
		
		n = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
		
		recvline[n] = 0;

		Fputs(recvline, stdout);
	}
}
