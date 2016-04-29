#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLINE 4096

int writef(char* dir,char* data){

        FILE * f;
        f = fopen(dir, "w+");
        if(f == NULL)
                printf("Can't open file!\n");

        fputs(data, f);

        fclose(f);

        return 0;
}

int checkCommond(char* comm){

    printf("comm %s\n", comm);



    if(strcmp(comm, "#n aaa")==0){
        printf("test a\n" );
        return 1;
    }

    if(strcmp(comm, "#p 123")==0){
        printf("testb\n" );
        return 2;
    }





    return 0;
}



int main(int argc, char** argv)
{
        int listenfd, connfd;
        struct sockaddr_in servaddr;
        char buff[4096];
        int n;

        //socket
        if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
                printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
                exit(0);
        }

        //server address
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(6769);

        //bind
        if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
                printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
                exit(0);
        }

        //active listen
        if( listen(listenfd, 10) == -1) {
                printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
                exit(0);
        }

        //开始等待客户端请求
        printf("======waiting for client's request======\n");
        while(1) {
                //链接
                if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1) {
                        printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
                        continue;
                }
                n = recv(connfd, buff, MAXLINE, 0);
                buff[n] = '\0';

                printf("recv msg from client\n");

                if(checkCommond(buff) >= 1){
                    send(connfd, "Right", strlen("Right"), 0);
                    close(connfd);
                    continue;
                }


                if (checkCommond(buff) == 0) {
                    writef("/Users/Rui/Desktop/tcp/temp3.txt", buff);
                    send(connfd, "Wellcome(from RuiWang14)", strlen("Wellcome(from RuiWang14)"), 0);
                    close(connfd);
                    continue;
                }

                if(checkCommond(buff) <= 1){
                    send(connfd, "False", strlen("False"), 0);
                    close(connfd);
                    continue;
                }


        }

        close(listenfd);
}
