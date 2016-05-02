/*
 * tcpsrv.c
 *
 * Author: Victoria Mengqi LIU
 * Compile: gcc -o srv tcpsrv.c
 * Usage: ./srv
 * Date: 29/4/16
 * Version 1.3
 */

#include "../lib/ial.h"
#include "../lib/wrap.c"
#include "../lib/writen.c"

int checkCMD(char* cmd)
{
    printf("CMD %s\n", cmd);
    
    
    if(strcmp(cmd, "#n aaa")==0){
        printf("test a\n" );
        return 1;
    }
    
    if(strcmp(cmd, "#p 123")==0){
        printf("testb\n" );
        return 2;
    }
    
    return 0;
}

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
        
        if (checkCMD(buff) >= 1)
        {
            send(connfd, "BINGO", strlen("BINGO"), 0);
            close(connfd);
            continue;
        }
        
        
        if (checkCMD(buff) == 0)
        {
            writen("/Users/Victoria/GitHub/Internet-Applications/lab6/tmp1", buff);
            send(connfd, "THIS IS VICKY'S SERVER", strlen("THIS IS VICKY'S SERVER"), 0);
            close(connfd);
            continue;

        }
        
        if (checkCMD(buff) <= 1)
        {
            send(connfd, "Ops", strlen("Ops"), 0);
            close(connfd);
            continue;
        }
        
//        writen("/Users/Victoria/GitHub/Internet-Applications/lab6/tmp1", buff);
//        send(connfd, "THIS IS VICKY'S SERVER", strlen("THIS IS VICKY'S SERVER"), 0);
//        close(connfd);
//        continue;

        
	}
    
    Close(listenfd);
}
