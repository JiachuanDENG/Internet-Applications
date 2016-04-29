#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLINE 4096

char* readf(char* dir){

        FILE * f;
        f = fopen(dir, "r");
        if(f == NULL)
                printf("Can't open file!\n");
        char s[100];

        fgets(s, 100, f);

        fclose(f);

        return s;
}

int main(int argc, char** argv)
{
        int sockfd, n;
        char recvline[4096], sendline[4096];
        struct sockaddr_in servaddr;
        char* str;

        if( argc != 2) {
                printf("usage: ./client <ipaddress>\n");
                exit(0);
        }

        //socket
        if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
                exit(0);
        }

        //server address
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(6666);
        if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
                printf("inet_pton error for %s\n",argv[1]);
                exit(0);
        }

        //connect
        if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
                printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
                exit(0);
        }


        printf("send msg to server\n");

        str = readf("/Users/Rui/Desktop/tcp/temp.txt");

        //fgets(sendline, 4096, stdin);
        if( send(sockfd, str, strlen(str), 0) < 0)
        {
                printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
                exit(0);
        }

        n = recv(sockfd, recvline, MAXLINE, 0);
        recvline[n] = '\0';
        printf("%s\n", recvline);

        close(sockfd);
        exit(0);
}
