// try_cli01.c
#include "../lib/ial.h"
#include "../lib/wrap.c"
#include "../lib/readn.c"

int main(int argc, char const *argv[])
{
	int sockfd, n;
	char recvline[MAXLINE], sendline[MAXLINE];
	struct sockaddr_in servaddr;
	char* str;
	char cmd[100];

    if (argc != 2)
    {
        printf("usage: udpcli <IPaddress>");
        exit(1);
    }
    
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	
	Inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	printf("=====Journey Start!=====\n");
	printf("Uploading......\n");

	str = readn("/Users/Victoria/GitHub/Internet-Applications/lab6/tmp1");

	Send(sockfd, str, strlen(str), 0);
	
	n = recv(sockfd, recvline, MAXLINE, 0);
	recvline[n] = '\0';
	printf("%s\n", recvline);

	close(sockfd);
	exit(0);
}
