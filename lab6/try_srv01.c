//	try_srv01.c
//
//	Author: Victoria Mengqi LIU

#include "../lib/ial.h"
#include "../lib/wrap.c"
#include "../lib/writen.c"

int main(int argc, char **argv)
{
	int listenfd, connfd;	
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	while(1)
	{
		clilen = sizeof(cliaddr);;
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
       
		if ( (childpid = Fork()) == 0)
		{
			Close(listenfd);
            str_echo(connfd);
			exit(0);
		}
		Close(connfd);
	}
}

void
str_echo(int sockfd)
{
    ssize_t		n;
    char		buf[MAXLINE];
    
again:
    while ( (n = read(sockfd, buf, MAXLINE)) > 0)
        recv(sockfd, buf, n);
    
    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        printf ("str_echo: read error");
}

