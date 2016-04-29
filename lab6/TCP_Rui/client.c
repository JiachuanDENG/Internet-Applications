#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLINE 4096

int has_login = 0;

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

int login(){

        int sockfd, n;
        char recvline[4096], sendline[4096];
        struct sockaddr_in servaddr;
        char* str;
        char commond[100];


        //socket
        if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
                exit(0);
        }

        //server address
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(6769);
        if( inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
                printf("inet_pton error for %s\n","127.0.0.1");
                exit(0);
        }

        //connect
        if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
                printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
                exit(0);
        }

        printf("====Wellcome!===\n");

        while(1) {

                printf("#Please Login!\n");
                char t_name[10],t_passw[10];
                printf("name: ");
                scanf("%s", t_name);

                char name[80];
                strcpy(name, "#n ");
                strcat(name, t_name);

                if( send(sockfd, name, strlen(name), 0) < 0)
                {
                        printf("2.send msg error: %s(errno: %d)\n", strerror(errno), errno);
                        exit(0);
                }

                n = recv(sockfd, recvline, MAXLINE, 0);
                recvline[n] = '\0';
                printf("%s\n", recvline);

                if(recvline[0]=='R') {
                        printf("%i\n",has_login );
                        return 1;
                }
        }
        close(sockfd);
}

int upload(){
        int sockfd, n;
        char recvline[4096], sendline[4096];
        struct sockaddr_in servaddr;
        char* str;
        char commond[100];

        //socket
        if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
                exit(0);
        }

        //server address
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(6769);
        if( inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
                printf("inet_pton error for %s\n","127.0.0.1");
                exit(0);
        }

        //connect
        if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
                printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
                exit(0);
        }

        printf("====Wellcome!===\n");


        printf("Uploading......\n");

        str = readf("/Users/Rui/Desktop/tcp/temp.txt");
        //printf("%s\n", str);

        //fgets(sendline, 4096, stdin);
        if( send(sockfd, str, strlen(str), 0) < 0){
                printf("1.send msg error: %s(errno: %d)\n", strerror(errno), errno);
                exit(0);
        }

        n = recv(sockfd, recvline, MAXLINE, 0);
        recvline[n] = '\0';
        printf("%s\n", recvline);


        close(sockfd);
        exit(0);
}


int main(int argc, char const *argv[]) {
    login();
    upload();
    return 0;
}
