//
// TCP CLIENT
//
// Author: Victoria Mengqi LIU
//
// Date: 06/06/16
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

#define PORT "8260"
#define MAXLINE 4096

int bytes_recv, bytes_sent, recv_len;
char recvline[MAXLINE];

void deal_recv(int sockfd, const char *restrict dir, void * buffer)
{
    FILE * file = fopen(dir, "a");
    if( fputs(buffer, file) == EOF ) {
        perror("[cli] fputs");
        exit(1);
    }
}

void stream_cli(const char * dir, int sockfd)
{
    // send
    bytes_sent = send(sockfd, dir, sizeof dir, 0);
    if ( bytes_sent == -1 ) {
        perror("[cli] send");
        exit(1);
    }
    //TEST
    else {
        printf("cli.stream_cli.send SUCCESS %d bytes_sent\tdir %s\tsizeof dir %lu\n", bytes_sent, dir, sizeof dir);
    }
  
    while(1) {
        printf("dir: %s", dir);
        recv_len = sizeof recvline;
        bytes_recv = recv(sockfd, recvline, recv_len-1, 0);
//        if ( bytes_recv <= 0 ) {
//            if ( bytes_recv == 0 ) {
//                printf("\ntcpcli: server socket %d hung up\n", sockfd);
//            } else {
//                perror("[cli] recv");
//            }
//            exit(1);
//        }
        // TEST
        printf("cli.stream_cli.recv TEST\trecvline: %s\tbytes_recv: %d\n", recvline, bytes_recv);
        if ( bytes_recv == -1 ) {
            perror("[cli] recv");
            exit(1);
        }
        deal_recv(sockfd, dir, recvline);
        
        printf("\tbytes recv: %d\n\tbytes stored: %d\n", bytes_recv, recv_len);
    }
}

int main(int argc, char * argv[])
{
    struct addrinfo hints, * res, * p;
    int status, sockfd;
    const char * srvport, * dir;
    
    if ( strcmp(argv[1], "-h") == 0 ) {
        if ( strcmp(argv[3], "-f") == 0 ) {
            srvport = PORT;
            dir = argv[4];
        } else if ( strcmp(argv[3], "-p") == 0 ) {
            if ( strcmp(argv[5], "-f") == 0 ) {
                srvport = argv[4];
                dir = argv[6];
            }
        }
    } else {
        fprintf(stderr, "\ntcpcli: illegal option\nUsage: tcpcli -h host [-p port] -f file");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ( (status = getaddrinfo(argv[2], srvport, &hints, &res)) != 0) {
        fprintf(stderr, "[cli] getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        // socket
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if ( sockfd == -1 ) {
            perror("[cli] socket");
            continue;
        }
        
        // connect
        if ( connect(sockfd, p->ai_addr, p->ai_addrlen) == -1 ) {
            close(sockfd);
            perror("[cli] connect");
            continue;
        }
        
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "[cli] failed to create socket");
        return 2;
    }
    
    stream_cli(dir, sockfd);
    
    freeaddrinfo(res);
    
    close(sockfd);
    return 0;
}

