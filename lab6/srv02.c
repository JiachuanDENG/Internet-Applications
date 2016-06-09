//
// TCP SERVER
//
// Author: Victoria Mengqi LIU
//
// Date: 06/06/16
//

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT "8260"
#define MAXLINE 4096
#define BACKLOG 10

void * get_in_addr(struct sockaddr * sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char * argv[])
{
    struct addrinfo hints, *res, *p;
    int status, sockfd, acceptfd;
    int bytes_recv, bytes_sent, readf_len;
    int yes = 1;
    const char *srvport;
    struct sockaddr_storage cli_addr;
    socklen_t cli_addrlen;
    char ch, ipstr[INET6_ADDRSTRLEN], readcli[MAXLINE], readf[MAXLINE];

    
    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    
    if (argc == 1) {
        srvport = PORT;
    } else if (argc == 3) {
        srvport = argv[2];
    } else {
        fprintf(stderr, "\ntcpsrv: illegal option\nUsage: tcpsrv [-p port]");
        exit(1);
    }
    
    
    if ( (status = getaddrinfo(NULL, srvport, &hints, &res)) != 0) {
        fprintf(stderr, "[srv] getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    
    for (p = res; p != NULL; p = p->ai_next) {
        // socket
        if ( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 ) {
            perror("[srv] socket");
            continue;
        }

        // setsockopt
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        // bind
        if ( bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("[srv] bind");
            continue;
        }

        break;
    }

    
    freeaddrinfo(res);
    
    
    if (p == NULL) {
        fprintf(stderr, "[srv] failed to bind socket\n");
        exit(1);
    }

    
    // listen
    if ( listen(sockfd, BACKLOG) == -1 ) {
        perror("[srv] listen");
        exit(1);
    }

    printf("\ntcpsrv: waiting for data on port TCP %s\n", srvport);
    
    while(1) {
        cli_addrlen = sizeof cli_addr;
        if ( (acceptfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_addrlen)) == -1 ) {
            perror("[srv] accept");
            exit(1);
        }
        
        printf("\ntcpsrv: from %s socket : %d\n",
                inet_ntop(cli_addr.ss_family,
                          get_in_addr((struct sockaddr *)&cli_addr),
                          ipstr, INET6_ADDRSTRLEN),
                acceptfd);
        
        if ( (bytes_recv = recv(acceptfd, readcli, MAXLINE, 0)) == -1 ) {
            perror("[srv] recv");
            exit(1);
        }
        printf("\ntcpsrv: %d bytes received.\n", bytes_recv);


        printf("readcli: %s\n", readcli);
        FILE *file = fopen(readcli, "rb");
        if (file == NULL) {
            if ( send(acceptfd, "\ntcpsrv: No such file\n", 22, 0) == -1 ) {
                close(acceptfd);
                perror("[srv] send");
            }
            perror("[srv] fopen");
            break;
        } else {
            printf("/nFile Open Success....../n");
        }
//        while ( fgets(readf, MAXLINE, file) != NULL ) {
//            // send file
//            if ( (bytes_sent = send(acceptfd, readf, strlen(readf), 0)) == -1 ) {
//                close(acceptfd);
//                perror("[srv] send");
//                exit(1);
//            }
//         
//            printf("\tbytes readf: %d\n\tbytes send: %d\n", readf_len, bytes_sent);
//        }

        while ( (ch = fgetc(file)) != EOF ) {
            write(acceptfd, &ch, strlen(&ch));
        }
        
        printf("fgetc() success\n");
        
        fclose(file);
    }
    
    close(acceptfd);
    close(sockfd);

    return 0;
}
       

