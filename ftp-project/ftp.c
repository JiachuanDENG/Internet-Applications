#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#define SERVER "172.20.10.11"
#define CLIENT "172.20.10.11"
#define PORT 1
#define PASV 2
#define STOR 3
#define RETR 4
#define MAXLINE 600

int bindAndListenSocket(int port) {
	int sock;
	struct sockaddr_in addr;
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket() failed\n");
		exit(0);
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	if ((bind(sock, (struct sockaddr *) &addr, sizeof(addr))) < 0) {
		printf("bind() failed\n");
		exit(0);
	}
	if (listen(sock, 15) < 0) {
		printf("listen() failed\n");
		exit(0);
	}
	return sock;
}

int acceptSocket(int proxySock) {
	struct sockaddr_in addr;
	socklen_t addr_len;
	addr_len = sizeof(addr);
	int sock;
	sock = accept(proxySock, (struct sockaddr *)&addr, &addr_len);
	if (sock < 0) {
		perror("accept() failed\n");
		exit(0);
	}
	return sock;
}

int connectTo(char *connect_ip, int port) {
	int sock;
	int addr_len;
	struct sockaddr_in addr;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket() failed\n");
		exit(0);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_aton(connect_ip, &addr.sin_addr);
	addr_len = sizeof(addr);
	if (connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr)) < 0) {
		printf("connect() failed\n");

	}
	return sock;
}

int isFileExist(char *file) {
    int fd = open(file, O_RDONLY);
    if (fd >= 0) {
        close(fd);
        return 1;
    } else {
        close(fd);
        return 0;
    }
}

int isCacheExist(char *file, int proxy_file_mode) {
	char *format;
	char tmp[200];
	strcpy(tmp, file);
	printf("file:%s\n",file);
	if (proxy_file_mode != RETR)
		return 0;
	if (strstr(tmp, ".") == NULL)
		return 0;
	if (isFileExist(tmp)) {
		return 0;
	}
	strtok(tmp, ".");
	format = strtok(NULL, ".");
	if (strcmp(format, "pdf") == 0 || strcmp(format, "jpg") == 0 || strcmp(format, "png") == 0) {
		return 1;
	}
	return 0;
}

void proxy_rw(char *file, int socket) {
	int fd = open(file, O_RDONLY);
	if (fd < 0) {
		exit(0);
	}
	else {
		char buf[MAXLINE];
		int readn;
		while ((readn = read(fd, buf, MAXLINE)) > 0) {
			write(socket, buf, readn);
		}
		close(fd);
	}
}

int main(int argc, const char *argv[])
{
	fd_set master, read_fds;
	struct timeval timeout;
	int proxy_cmd_socket = 0;
	int accept_cmd_socket = 0;
	int connect_cmd_socket = 0;
	int proxy_data_socket = 0;
	int accept_data_socket = 0;
	int connect_data_socket = 0;
	int proxy_data_mode;
	int proxy_data_port;
	int selectResult = 0;
	int fdmax = 10;
	int proxy_file_mode;
	int cache = 0;
	int bytes_creat;
	char file[200];

	FD_ZERO(&master);
	bzero(&timeout, sizeof(timeout));
	proxy_cmd_socket = bindAndListenSocket(21);
	FD_SET(proxy_cmd_socket, &master);
	timeout.tv_sec = 600;
	timeout.tv_usec = 0;

	while (1) {
		FD_ZERO(&read_fds);
		memcpy(&read_fds, &master, sizeof(master));
		selectResult = select(fdmax, &read_fds, NULL, NULL, &timeout);
		// fail
		if (selectResult < 0) {
			perror("select() failed\n");
			exit(0);
		}
		// timeout
		if (selectResult == 0) {
			printf("select() timed out.\n");
			continue;
		}		
		int i;
		for (i = 0; i < fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == proxy_cmd_socket) {
					if (FD_ISSET(connect_cmd_socket, &master)) {
						close(connect_cmd_socket);
						FD_CLR(connect_cmd_socket, &master);
					}
					if (FD_ISSET(accept_cmd_socket, &master)) {
						close(accept_cmd_socket);
						FD_CLR(accept_cmd_socket, &master);
					}
					connect_cmd_socket = connectTo(SERVER, 21);
					accept_cmd_socket = acceptSocket(proxy_cmd_socket); 										
					FD_SET(accept_cmd_socket, &master);
					FD_SET(connect_cmd_socket, &master);
				}

				if (i == accept_cmd_socket) {
					char buf[MAXLINE];
					int bytes_read;
					if ((bytes_read = read(i, buf, MAXLINE)) == 0) {
						FD_CLR(i, &master);
						FD_CLR(connect_cmd_socket, &master);
						close(i); 
						close(connect_cmd_socket);
					}
					else {
						if (strncmp(buf, "PORT", 4) == 0) {
							strtok(buf, ",");
							strtok(NULL, ",");
							strtok(NULL, ",");
							strtok(NULL, ",");
							if (FD_ISSET(proxy_data_socket, &master)) {
								close(proxy_data_socket);
								FD_CLR(proxy_data_socket, &master);
							}
							char *tsp;
							int p1, p2;
							tsp = strtok(NULL, ",");
							p1 = atoi(tsp);
							tsp = strtok(NULL, ",");
							p2 = atoi(tsp);
							sprintf(buf, "PORT 10,37,129,3,%d,%d\r\n", p1, p2);
							bytes_read = strlen(buf);
							proxy_data_mode = PORT;
							proxy_data_port = p1 * 256 + p2;
							proxy_data_socket = bindAndListenSocket(proxy_data_port);
							FD_SET(proxy_data_socket, &master);
						}

						//RETR
						if (strncmp(buf, "RETR", 4) == 0) {
							proxy_file_mode = RETR;
							buf[bytes_read] = '\0';
							char *name = strtok(buf + 5, "\r");
							strcpy(file, name);
							if (isFileExist(file)) {
								if (proxy_data_mode == PORT) {
									char before[200];
									char after[200];
									sprintf(before,"150 Opening data channel for file download from server of \"/%s\"\r\n", file);
									write(accept_cmd_socket, before, strlen(before));
									connect_data_socket = connectTo(CLIENT, proxy_data_port);
									proxy_rw(file, connect_data_socket);
									sprintf(after, "226 Successfully transferred \"/%s\"\r\n", file);
									write(accept_cmd_socket, after, strlen(after));
									close(connect_data_socket);
								}
								continue;
							}
						}
						//STOR
						if (strncmp(buf, "STOR", 4) == 0) {
							proxy_file_mode = STOR;
						}

						write(connect_cmd_socket, buf, bytes_read);
					}
				}

				if (i == connect_cmd_socket) {
					char buf[MAXLINE];
					int bytes_read;
					bytes_read = read(i, buf, MAXLINE);
					if (bytes_read == 0) {
						close(i); 
						FD_CLR(i, &master);
						close(accept_cmd_socket);
						FD_CLR(accept_cmd_socket, &master);
					}
					else {
						if (strncmp(buf, "227", 3) == 0) {
							if (FD_ISSET(proxy_data_socket, &master)) {
								close(proxy_data_socket);
								FD_CLR(proxy_data_socket, &master);
							}
							strtok(buf, ",");
							strtok(NULL, ",");
							strtok(NULL, ",");
							strtok(NULL, ",");
							char *tsp;
							int p1, p2;
							tsp = strtok(NULL, ",");
							p1 = atoi(tsp);
							tsp = strtok(NULL, ",");
							p2 = atoi(tsp);

							sprintf(buf, "227 Entering Passive Mode (10,37,129,3,%d,%d)\r\n", p1, p2);
							bytes_read = strlen(buf);
							proxy_data_mode = PASV;
							proxy_data_port = p1 * 256 + p2;
							proxy_data_socket = bindAndListenSocket(proxy_data_port);
							FD_SET(proxy_data_socket, &master);
						}
						write(accept_cmd_socket, buf, bytes_read);
					}
				}

				if (i == proxy_data_socket) {
					if (proxy_data_mode == PORT) {
						accept_data_socket = acceptSocket(proxy_data_socket);
						connect_data_socket = connectTo(CLIENT, proxy_data_port);
						FD_SET(accept_data_socket, &master);
						FD_SET(connect_data_socket, &master);
						if (!isCacheExist(file, proxy_file_mode)) {
							cache = 0;
						} else {
							bytes_creat = creat(file, 0644);
							if (bytes_creat < 0) {
								exit(0);
							}
							cache = 1;
						}
					}
					else {
						if (proxy_file_mode != RETR) {
							accept_data_socket = acceptSocket(proxy_data_socket);
							connect_data_socket = connectTo(SERVER, proxy_data_port);
							FD_SET(accept_data_socket, &master);
							FD_SET(connect_data_socket, &master);
						}
						else {
							if (isFileExist(file)) {
								accept_data_socket = acceptSocket(proxy_data_socket);
								char before[200];
								sprintf(before, "150 Opening data channel for file download from server of \"/%s\"\r\n", file);
								write(accept_cmd_socket, before, strlen(before));
								proxy_rw(file, accept_data_socket);
								char after[200];
								sprintf(after, "226 Successfully transferred \"/%s\"\r\n", file);
								write(accept_cmd_socket, after, strlen(after));
								close(accept_data_socket);
							}
							else {
								connect_data_socket = connectTo(SERVER, proxy_data_port);
								accept_data_socket = acceptSocket(proxy_data_socket);
								FD_SET(accept_data_socket, &master);
								FD_SET(connect_data_socket, &master);
								if (!isCacheExist(file, proxy_file_mode)) {
									cache = 0;
								}
								else {
									cache = 1;
									bytes_creat = creat(file, 0644);
									if (bytes_creat < 0) {
										exit(0);
									}
								}
							}
						}
					}
				}

				if (i == accept_data_socket) {
					char buf[MAXLINE];
					int bytes_read;
					bytes_read = read(i, buf, MAXLINE);
					if (bytes_read == 0) {
						if (cache) {
							close(bytes_creat);
						}
						close(i);
						FD_CLR(accept_data_socket, &master);
						close(connect_data_socket);
						FD_CLR(connect_data_socket, &master);
					}
					else {
						if (cache) {
							write(bytes_creat, buf, bytes_read);
						}
						write(connect_data_socket, buf, bytes_read);
					}

				}

				if (i == connect_data_socket) {
					char buf[MAXLINE];
					int bytes_read;
					bytes_read = read(i, buf, MAXLINE);
					if (bytes_read == 0) {
						if (cache) {
							close(bytes_creat);
						}
						close(i);
						close(accept_data_socket);
						FD_CLR(accept_data_socket, &master);
						FD_CLR(connect_data_socket, &master);
					}
					else {
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
