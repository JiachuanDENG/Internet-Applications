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
#include <fcntl.h>

#define CLI "172.20.10.11"
#define SRV "172.20.10.11"
// #define IP "172.20.10.11"     // PROXY NAT "10.211.55.3" Host-only "10.37.129.3"
#define PROXY_CMD_PORT "21"
#define ACCEPT_CMD_PORT "21"
#define CONNECT_DATA_PORT "20"
#define SRV_CMD_PORT "21"
#define CLI_DATA_PORT "20"
#define MAXLINE 4096
#define PORT 1
#define PASV 2
#define STOR 3
#define RETR 4
#define COPYMODE 0644
#define BACKLOG 10


int connect_cmd_port = 6280;
int proxy_data_port = 8260;
int proxy_data_mode;
int proxy_file_mode;


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


int bindAndListenSocket(char *port)
{
    struct addrinfo hints, *res, *p;
    int status;
    int sockfd;
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
    
    
    freeaddrinfo(res);
    if (p == NULL) {
        fprintf(stderr, "proxy: failed to bind\n");
        exit(2);
    }
    
    
    // listen
    if ( listen(sockfd, BACKLOG) == -1) error("listen");
    
    
    return sockfd;
}


int connectTo(char *ip, char *port)
{
    struct addrinfo hints, *res, *p;
    int status;
    int sockfd;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    
    if ( (status = getaddrinfo(ip, port, &hints, &res)) != 0 ) {
        fprintf(stderr, "[cli] getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }
    
    
    for (p = res; p != NULL; p = p->ai_next) {
        if ( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 ) {
            perror("[cli] socket");
            continue;
        }
        
        if ( connect(sockfd, p->ai_addr, p->ai_addrlen) == -1 ) {
            close(sockfd);
            perror("[cli] connect");
            continue;
        }
        
        break;
    }
    
    
    freeaddrinfo(res);
    if (p == NULL) {
        fprintf(stderr, "[cli] failed to create socket");
        exit(1);
    }
    
    
    return sockfd;
}


int isFileExist(char *file)
{
    int fd = open(file, O_RDONLY);
    if ( fd >= 0 ) {
        close(fd);
        return 1;
    } else {
        close(fd);
        return 0;
    }
}


void proxy_rw(int sockfd, char *file)
{
    int fd, bytes_read;
    char buf[MAXLINE];
    
    
    fd = open(file, O_RDONLY);
    if ( fd < 0 ) exit(0);
    else {
        while ( (bytes_read = read(fd, buf, MAXLINE)) > 0 ) {
            write(sockfd, buf, bytes_read);
        }
        close(fd);
    }
}


int isCacheExist(char *file, int proxy_file_mode)
{
    char *format;
    char tmp[100];
    
    
    strcpy(tmp, file);
    printf("file:%s\n", file);
    
    if (proxy_file_mode != RETR) return 0;
    if (strstr(tmp, ".") == NULL) return 0;
    if (isFileExist(tmp)) return 0;
    
    strtok(tmp, ".");
    format = strtok(NULL, ".");
    if (strcmp(format, "pdf") == 0 || strcmp(format, "jpg") == 0 || strcmp(format, "png") == 0) return 1;
    
    return 0;
}


int acceptSocket(int sockfd)
{
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    int newsockfd;
    
    addrlen = sizeof remoteaddr;
    newsockfd = accept(sockfd, (struct sockaddr *)&remoteaddr, &addrlen);
    if (newsockfd < 0) error("accept");
    
    return newsockfd;
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
    
    int nfds, i, j, p1, p2, cache;
    char buf[MAXLINE], file[100], ipstr[INET6_ADDRSTRLEN];
    
    int bytes_read, bytes_write, bytes_creat, numbytes;
    
    
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    
    proxy_cmd_socket = bindAndListenSocket(PROXY_CMD_PORT);
    
    
    // add socket to master set
    FD_SET(proxy_cmd_socket, &master);
    
    // keep trak of the biggest file descriptor
    fdmax = proxy_cmd_socket;
    
    
    // main loop
    while (1) {
        FD_ZERO(&read_fds);
        memcpy(&read_fds, &master, sizeof master);
        if ( select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1 ) error("select");
        
        
        for (i = 0; i < fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == proxy_cmd_socket) {
                    accept_cmd_socket = acceptSocket(proxy_cmd_socket);
                    
                    FD_SET(accept_cmd_socket, &master);
                    if ( accept_cmd_socket > fdmax ) {
                        fdmax = accept_cmd_socket;
                    }
                    
                    connect_cmd_socket = connectTo(SRV, SRV_CMD_PORT);
                    
                    FD_SET(connect_cmd_socket, &master);
                }
                
                
                if (i == accept_cmd_socket) {
                    bytes_read = read(i, buf, MAXLINE);
                    
                    if ( bytes_read == 0 ) {
                        close(i);
                        close(connect_cmd_socket);
                        
                        FD_CLR(i, &master);
                        FD_CLR(connect_cmd_socket, &master);
                    } else {
                        // PORT
                        if ( strncmp(buf, "PORT", 4) == 0 ) {
                            strtok(buf, ",");
                            strtok(NULL, ",");
                            strtok(NULL, ",");
                            strtok(NULL, ",");
                            if (FD_ISSET(proxy_data_socket, &master)) {
                                close(proxy_data_socket);
                                FD_CLR(proxy_data_socket, &master);
                            }
                            
                            char *tmp;
                            tmp = strtok(NULL, ",");
                            p1 = atoi(tmp);
                            tmp = strtok(NULL, ",");
                            p2 = atoi(tmp);
                            sprintf(buf, "PORT 10,37,129,3,%d,%d\n", p1, p2);
                            bytes_read = strlen(buf);
                            proxy_data_mode = PORT;
                            proxy_data_port = p1*256 + p2;
                            
                            char proxy_data_port = (char) proxy_data_port;
                            proxy_data_socket = bindAndListenSocket(&proxy_data_port);
                            FD_SET(proxy_data_socket, &master);
                        }
                        
                        // RETR
                        if ( strncmp(buf, "RETR", 4) == 0 ) {
                            proxy_file_mode = RETR;
                            buf[bytes_read] = '\0';
                            
                            char *tmp = strtok(buf+5, "\r");
                            strcpy(file, tmp);
                            if (isFileExist(file)) {
                                if (proxy_data_mode == PORT) {
                                    char prv[200];
                                    char aft[200];
                                    sprintf(prv, "150 Opening data channel for file download from server of \"/%s\"\n", file);
                                    write(accept_cmd_socket, prv, strlen(prv));
                                    
                                    char proxy_data_port = (char) proxy_data_port;
                                    connect_data_socket = connectTo(CLI, &proxy_data_port);
                                    proxy_rw(connect_data_socket, file);
                                    sprintf(aft, "226 Successfully transferred \"/%s\"\n", file);
                                    write(accept_cmd_socket, aft, strlen(aft));
                                    
                                    close(connect_data_socket);
                                }
                                continue;
                            }
                        }
                        
                        // STOR
                        if ( strncmp(buf, "STOR", 4) == 0 ) {
                            proxy_file_mode = STOR;
                        }
                        
                        
                        write(connect_cmd_socket, buf, bytes_read);
                    }
                }
                
                
                if (i == connect_cmd_socket) {
                    bytes_read = read(i, buf, MAXLINE);
                    
                    if ( bytes_read <= 0 ) {
                        if ( bytes_read == 0 ) {
                            printf("proxy: socket %d hung up\n", i);
                        } else {
                            perror("read");
                        }
                        close(i);
                        close(connect_cmd_socket);
                        
                        FD_CLR(i, &master);
                        FD_CLR(connect_cmd_socket, &master);
                    } else {
                        if (strncmp(buf, "227", 3) == 0) {
                            if(FD_ISSET(proxy_data_socket, &master)) {
                                close(proxy_data_socket);
                                FD_CLR(proxy_data_socket, &master);
                                
                                strtok(buf, ",");
                                strtok(NULL, ",");
                                strtok(NULL, ",");
                                strtok(NULL, ",");
                                
                                char *tmp;
                                tmp = strtok(NULL, ",");
                                p1 = atoi(tmp);
                                tmp = strtok(NULL, ",");
                                p2 = atoi(tmp);
                                
                                sprintf(buf, "227 Entering Passive Mode (10,37,129,3,%d,%d)\n", p1, p2);
                                
                                bytes_read = strlen(buf);
                                proxy_data_mode = PASV;
                                proxy_data_port = p1*256 + p2;
                                
                                char proxy_data_port = (char) proxy_data_port;
                                proxy_data_socket = bindAndListenSocket(&proxy_data_port);
                                FD_SET(proxy_data_socket, &master);
                            }
                            write(accept_cmd_socket, buf, bytes_read);
                        }
                    }
                }
                
                
                if (i == proxy_data_socket) {
                    if (proxy_data_mode == PORT) {
                        accept_data_socket = acceptSocket(proxy_data_socket);

                        FD_SET(accept_data_socket, &master);
                        if ( accept_cmd_socket > fdmax ) {
                            fdmax = accept_cmd_socket;
                        }
                        
                        char proxy_data_port = (char) proxy_data_port;
                        connect_data_socket = connectTo(CLI, &proxy_data_port);
                        
                        FD_SET(connect_data_socket, &master);
                        
                        if (!isCacheExist(file, proxy_file_mode)) cache = 0;
                        else {
                            bytes_creat = creat(file, COPYMODE);
                            if ( bytes_creat < 0 ) exit(0);
                            
                            cache = 1;
                        }
                    } else {
                        if (proxy_file_mode != RETR) {
                            accept_data_socket = acceptSocket(proxy_data_socket);
                            
                            FD_SET(accept_data_socket, &master);
                            if ( accept_cmd_socket > fdmax ) {
                                fdmax = accept_cmd_socket;
                            }
                            
                            char proxy_data_port = (char) proxy_data_port;
                            connect_data_socket = connectTo(SRV, &proxy_data_port);
                            
                            FD_SET(connect_data_socket, &master);
                        } else {
                            if ( isFileExist(file)) {
                                accept_data_socket = acceptSocket(proxy_data_socket);
                                FD_SET(accept_data_socket, &master);
                                if ( accept_cmd_socket > fdmax ) {
                                    fdmax = accept_cmd_socket;
                                }
                                
                                char prv[200];
                                char aft[200];
                                sprintf(prv, "150 Opening data channel for file download from server of \"/%s\"\n", file);
                                write(accept_cmd_socket, prv, strlen(prv));
                                proxy_rw(accept_data_socket, file);
                                
                                sprintf(aft, "226 Successfully transferred \"/%s\"\n", file);
                                write(accept_cmd_socket, aft, strlen(aft));
                                close(accept_data_socket);
                            } else {
                                accept_data_socket = acceptSocket(proxy_data_socket);
                                FD_SET(accept_data_socket, &master);
                                if ( accept_cmd_socket > fdmax ) {
                                    fdmax = accept_cmd_socket;
                                }
                                
                                char proxy_data_port = (char) proxy_data_port;
                                connect_data_socket = connectTo(SRV, &proxy_data_port);
                                
                                FD_SET(connect_data_socket, &master);
                                
                                if (!isCacheExist(file, proxy_file_mode)) cache = 0;
                                else {
                                    cache = 1;
                                    bytes_creat = creat(file, COPYMODE);
                                    if ( bytes_creat < 0 ) exit(0);
                                }
                            }
                        }
                    }
                }
                
                
                if (i == accept_data_socket) {
                    bytes_read = read(i, buf, MAXLINE);
                    if ( bytes_read == 0) {
                        if (cache) close(bytes_creat);
                        
                        close(i);
                        close(connect_data_socket);
                        FD_CLR(accept_data_socket, &master);
                        FD_CLR(connect_data_socket, &master);
                    } else {
                        if (cache) {
                            write(bytes_creat, buf, bytes_read);
                        }
                        write(connect_data_socket, buf, bytes_read);
                    }
                }
                
                
                if (i == connect_data_socket) {
                    bytes_read = read(i, buf, MAXLINE);
                    if ( bytes_read == 0 ) {
                        if (cache) close(bytes_creat);
                        
                        close(i);
                        close(accept_data_socket);
                        FD_CLR(accept_data_socket, &master);
                        FD_CLR(connect_data_socket, &master);
                    } else {
                        if (cache) {
                            write(bytes_creat, buf, bytes_read);
                        }
                        write(accept_data_socket, buf, bytes_read);
                    }
                }
            }
        }
    }
    
    return 0;
}
