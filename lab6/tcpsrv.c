//	try_srv01.c
//
//	Author: Victoria Mengqi LIU

#include "../lib/ial.h"
#include "../lib/wrap.c"
#include "../lib/writen.c"

int main(int argc, char **argv)
{
	int listenfd, connfd;
	struct sockaddr_in servaddr;
    char buff[MAXLINE];
    int n;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);
    
    printf("=====WAITING FOR REQ=====\n");

	while(1)
	{
		connfd = Accept(listenfd, (SA *) NULL, NULL);
        
        n = Recv(connfd, buff, MAXLINE, 0);
        buff[n] = '\0';
        
        printf("RECVING...");
        
        writen("/Users/Victoria/GitHub/Internet-Applications/lab6/tmp2", buff);
        Send(connfd, "THIS IS VICKY'S SERVER", strlen("THIS IS VICKY'S SERVER"), 0);
        
		Close(connfd);
        continue;
	}
    
    Close(listenfd);
}
