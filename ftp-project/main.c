//
// Description:
//
// Author: Victoria Mengqi LIU
//
// Date: 17/06/16
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ACCEPT_CMD_PORT 21
#define CONNECT_DATA_PORT 20

void error(const char *msg)
{
    perror(msg);
    exit(0);
}


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}


int bindAndListenSocket(int sockfd, int port)
{
    struct addrinfo hints, *res, *p;
    int status;
    int optval = 1;         // for setsockopt() SO_REUSEADDR
    
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    
    if ( (status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        fprintf(stderr, "proxy: %s\n", gai_strerror(status));
        exit(1);
    }
    
    
    for (p = res; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if ( sockfd < 0 ) {
            continue;
        }
        
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
        
        if ( bind(sockfd, p->ai_addr, p->ai_addrlen) < 0 ) {
            close(sockfd);
            continue;
        }
        
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }
    
    freeaddrinfo(res);
    
    
    // listen
    if ( listen(sockfd, BACKLOG) == -1) error("listen");
    
    
    return sockfd;
}


int main(int argc, const char *argv[])
{
    fd_set master;
    fd_set read_fds;
    int fdmax;
    
    int proxy_cmd_socket;
    int accept_cmd_socket;
    int connect_cmd_socket;
    int proxy_data_socket;
    int accept_data_socket;
    int connect_data_socket;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    
    int optval = 1;         // for setsockopt() SO_REUSEADDR
    int nfds, i, j;
    
    struct addrinfo hints, *res, *p;
    
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    
    proxy_cmd_socket = bindAndListenSocket(proxy_cmd_socket, 21);
    
    
    // add socket to master set
    FD_SET(proxy_cmd_socket, &master);
    
    // keep trak of the biggest file descriptor
    fdmax = proxy_cmd_socket

    
    // main loop
    while (TRUE) {
        read_fds = master;
        if ( select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1 ) {
            perror("select");
            exit(3);
        }
        

        for (i = 0; i < fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == proxy_cmd_socket) {
                    addrlen = sizeof remoteaddr;
                    accept_cmd_socket = accept(proxy_cmd_socket, (struct sockaddr *)&remoteaddr, &addrlen);
                    
                    if ( accept_cmd_socket == -1 ) {
                        perror("accept");
                    } else {
                        FD_SET(accept_cmd_socket, &master);
                        if ( accept_cmd_socket > fdmax ) {
                            fdmax = accept_cmd_socket;
                        }
                    }
                    
                    connect_cmd_socket = connectToServer(); //执行connect操作,建立proxy和服务器端之间的控制连接
                    
                    //将新得到的socket加入到master结合中
                    FD_SET(accept_cmd_socket, &master);
                    FD_SET(connect_cmd_socket, &master);
                }
                
                if (i == accept_cmd_socket) {
                    char buff[BUFFSIZE] = {0};
                   
                    if (read(i, buff, BUFFSIZE) == 0) {
                        close(i); //如果接收不到内容,则关闭Socket
                        close(connect_cmd_socket);
                        
                        //socket关闭后，使用FD_CLR将关闭的socket从master集合中移去,使得select函数不再监听关闭的socket
                        FD_CLR(i, &master);
                        FD_CLR(connect_cmd_socket, &master);

                    } else {
                        //如果接收到内容,则对内容进行必要的处理，之后发送给服务器端（写入connect_cmd_socket）

                        //处理客户端发给proxy的request，部分命令需要进行处理，如PORT、RETR、STOR                        
                        //PORT
                        //////////////
                        
                        //RETR
                        //////////////
                        
                        //STOR
                        //////////////
                        
                        
                        //写入proxy与server建立的cmd连接,除了PORT之外，直接转发buff内容
                        write((connect_cmd_socket), buff, strlen(buff));
                    }
                }
                
                if (i == connect_cmd_socket) {
                  //处理服务器端发给proxy的reply，写入accept_cmd_socket
                    
                  //PASV收到的端口 227 （port）
                  //////////////
                }
                
                if (i == proxy_data_socket) {
                  //建立data连接(accept_data_socket、connect_data_socket)
                }
                
                if (i == accept_data_socket) {
                    //判断主被动和传输方式（上传、下载）决定如何传输数据
                }
                
                if (i == connect_data_socket) {
                    //判断主被动和传输方式（上传、下载）决定如何传输数据
                }
            }
        }
    }
    
    return 0;
}
