#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#define SERVER "172.20.10.3"
#define CLIENT "172.20.10.3"
#define PORT 1
#define PASV 2
#define STOR 3
#define RETR 4
#define BUFFSIZE 600

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
	int addr_len;
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

int ifCache(char *filename, int file_way) {
	char *wei;
	char t_name[200];
	strcpy(t_name, filename);
	printf("file:%s\n",filename);
	if (file_way != RETR)
		return 0;
	if (strstr(t_name, ".") == NULL)
		return 0;
	if (openTestExist(t_name)) {
		return 0;
	}
	strtok(t_name, ".");
	wei = strtok(NULL, ".");
	if (strcmp(wei, "pdf") == 0 || strcmp(wei, "jpg") == 0 || strcmp(wei, "png") == 0) {
		return 1;
	}
	return 0;
}

int openTestExist(char *filename) {
	int fd = open(filename, O_RDONLY);
	if (fd >= 0) {
		close(fd);
		return 1;
	}
	if (fd < 0) {
		close(fd);
		return 0;
	}
}

int readAndSend(char *filename, int socket) {
	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		exit(0);
	}
	else {
		char buff[BUFFSIZE];
		int readn;
		while ((readn = read(fd, buff, BUFFSIZE)) > 0) {
			write(socket, buff, readn);
		}
		close(fd);
	}
}

int main(int argc, const char *argv[])
{
	fd_set master_set, working_set;
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
	int select_sd = 10;
	int file_way;
	int download_cache = 0;
	int write_file;
	char cmd_file[200];

	FD_ZERO(&master_set);
	bzero(&timeout, sizeof(timeout));
	proxy_cmd_socket = bindAndListenSocket(21);
	FD_SET(proxy_cmd_socket, &master_set);
	timeout.tv_sec = 600;
	timeout.tv_usec = 0;

	while (1) {
		FD_ZERO(&working_set);
		memcpy(&working_set, &master_set, sizeof(master_set));
		selectResult = select(select_sd, &working_set, NULL, NULL, &timeout);
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
		for (i = 0; i < select_sd; i++) {
			if (FD_ISSET(i, &working_set)) {
				if (i == proxy_cmd_socket) {
					if (FD_ISSET(connect_cmd_socket, &master_set)) {
						close(connect_cmd_socket);
						FD_CLR(connect_cmd_socket, &master_set);
					}
					if (FD_ISSET(accept_cmd_socket, &master_set)) {
						close(accept_cmd_socket);
						FD_CLR(accept_cmd_socket, &master_set);
					}
					connect_cmd_socket = connectTo(SERVER, 21);
					accept_cmd_socket = acceptSocket(proxy_cmd_socket); 										
					FD_SET(accept_cmd_socket, &master_set);
					FD_SET(connect_cmd_socket, &master_set);
				}

				if (i == accept_cmd_socket) {
					char buff[BUFFSIZE];
					int read_l;
					if ((read_l = read(i, buff, BUFFSIZE)) == 0) {
						FD_CLR(i, &master_set);
						FD_CLR(connect_cmd_socket, &master_set);
						close(i); 
						close(connect_cmd_socket);
					}
					else {
						if (strncmp(buff, "PORT", 4) == 0) {
							strtok(buff, ",");
							strtok(NULL, ",");
							strtok(NULL, ",");
							strtok(NULL, ",");
							if (FD_ISSET(proxy_data_socket, &master_set)) {
								close(proxy_data_socket);
								FD_CLR(proxy_data_socket, &master_set);
							}
							char *tsp;
							int p1, p2;
							tsp = strtok(NULL, ",");
							p1 = atoi(tsp);
							tsp = strtok(NULL, ",");
							p2 = atoi(tsp);
							sprintf(buff, "PORT 10,37,129,4,%d,%d\r\n", p1, p2);
							read_l = strlen(buff);
							proxy_data_mode = PORT;
							proxy_data_port = p1 * 256 + p2;
							proxy_data_socket = bindAndListenSocket(proxy_data_port);
							FD_SET(proxy_data_socket, &master_set);
						}

						//RETR
						if (strncmp(buff, "RETR", 4) == 0) {
							file_way = RETR;
							buff[read_l] = '\0';
							char *name = strtok(buff + 5, "\r");
							strcpy(cmd_file, name);
							if (openTestExist(cmd_file)) {
								if (proxy_data_mode == PORT) {
									char before[200];
									char after[200];
									sprintf(before,"150 Opening data channel for file download from server of \"/%s\"\r\n", cmd_file);
									write(accept_cmd_socket, before, strlen(before));
									connect_data_socket = connectTo(CLIENT, proxy_data_port);
									readAndSend(cmd_file, connect_data_socket);
									sprintf(after, "226 Successfully transferred \"/%s\"\r\n", cmd_file);
									write(accept_cmd_socket, after, strlen(after));
									close(connect_data_socket);
								}
								continue;
							}
						}
						//STOR
						if (strncmp(buff, "STOR", 4) == 0) {
							file_way = STOR;
						}

						write(connect_cmd_socket, buff, read_l);
					}
				}

				if (i == connect_cmd_socket) {
					char buff[BUFFSIZE];
					int read_l;
					read_l = read(i, buff, BUFFSIZE);
					if (read_l == 0) {
						close(i); 
						FD_CLR(i, &master_set);
						close(accept_cmd_socket);
						FD_CLR(accept_cmd_socket, &master_set);
					}
					else {
						if (strncmp(buff, "227", 3) == 0) {
							if (FD_ISSET(proxy_data_socket, &master_set)) {
								close(proxy_data_socket);
								FD_CLR(proxy_data_socket, &master_set);
							}
							strtok(buff, ",");
							strtok(NULL, ",");
							strtok(NULL, ",");
							strtok(NULL, ",");
							char *tsp;
							int p1, p2;
							tsp = strtok(NULL, ",");
							p1 = atoi(tsp);
							tsp = strtok(NULL, ",");
							p2 = atoi(tsp);

							sprintf(buff, "227 Entering Passive Mode (10,37,129,4,%d,%d)\r\n", p1, p2);
							read_l = strlen(buff);
							proxy_data_mode = PASV;
							proxy_data_port = p1 * 256 + p2;
							proxy_data_socket = bindAndListenSocket(proxy_data_port);
							FD_SET(proxy_data_socket, &master_set);
						}
						write(accept_cmd_socket, buff, read_l);
					}
				}

				if (i == proxy_data_socket) {
					if (proxy_data_mode == PORT) {
						accept_data_socket = acceptSocket(proxy_data_socket);
						connect_data_socket = connectTo(CLIENT, proxy_data_port);
						FD_SET(accept_data_socket, &master_set);
						FD_SET(connect_data_socket, &master_set);
						if (!ifCache(cmd_file, file_way)) {
							download_cache = 0;
						} else {
							write_file = creat(cmd_file, 0644);
							if (write_file < 0) {
								exit(0);
							}
							download_cache = 1;
						}
					}
					else {
						if (file_way != RETR) {
							accept_data_socket = acceptSocket(proxy_data_socket);
							connect_data_socket = connectTo(SERVER, proxy_data_port);
							FD_SET(accept_data_socket, &master_set);
							FD_SET(connect_data_socket, &master_set);
						}
						else {
							if (openTestExist(cmd_file)) {
								accept_data_socket = acceptSocket(proxy_data_socket);
								char before[200];
								sprintf(before, "150 Opening data channel for file download from server of \"/%s\"\r\n", cmd_file);
								write(accept_cmd_socket, before, strlen(before));
								readAndSend(cmd_file, accept_data_socket);
								char after[200];
								sprintf(after, "226 Successfully transferred \"/%s\"\r\n", cmd_file);
								write(accept_cmd_socket, after, strlen(after));
								close(accept_data_socket);
							}
							else {
								connect_data_socket = connectTo(SERVER, proxy_data_port);
								accept_data_socket = acceptSocket(proxy_data_socket);
								FD_SET(accept_data_socket, &master_set);
								FD_SET(connect_data_socket, &master_set);
								if (!ifCache(cmd_file, file_way)) {
									download_cache = 0;
								}
								else {
									download_cache = 1;
									write_file = creat(cmd_file, 0644);
									if (write_file < 0) {
										exit(0);
									}
								}
							}
						}
					}
				}

				if (i == accept_data_socket) {
					char buff[BUFFSIZE];
					int read_l;
					read_l = read(i, buff, BUFFSIZE);
					if (read_l == 0) {
						if (download_cache) {
							close(write_file);
						}
						close(i);
						FD_CLR(accept_data_socket, &master_set);
						close(connect_data_socket);
						FD_CLR(connect_data_socket, &master_set);
					}
					else {
						if (download_cache) {
							write(write_file, buff, read_l);
						}
						write(connect_data_socket, buff, read_l);
					}

				}

				if (i == connect_data_socket) {
					char buff[BUFFSIZE];
					int read_l;
					read_l = read(i, buff, BUFFSIZE);
					if (read_l == 0) {
						if (download_cache) {
							close(write_file);
						}
						close(i);
						close(accept_data_socket);
						FD_CLR(accept_data_socket, &master_set);
						FD_CLR(connect_data_socket, &master_set);
					}
					else {
						if (download_cache) {
							write(write_file, buff, read_l);
						}
						write(accept_data_socket, buff, read_l);
					}
				}
			}
		}
	}
	return 0;
}
