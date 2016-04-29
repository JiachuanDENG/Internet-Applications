// try_cli01.c
#include "../lib/ial.h"

int
main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2)
		printf ("usage: tcpcli <IPaddress>");
		exit(1);
	
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bezro(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockdf, (SA *) &servaddr, sizeof(servaddr));

	echo_cli(stdin, sockfd);

	exit(0);
}

void
echo_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != NULL)
	{
		Writen(sockfd, sendline, strlen(sendline));

		if(Readline(sockfd, recvline, MAXLINE) == 0)
			printf ("echo_cli: server terminated prematurely");
			exit(1);

		Fputs(recvline, stdout);
	}
}

void
Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nybtes)
		printf("writen error");
		exit(1);
}

void writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ( (nwritten = write(fd, ptr, nleft)) <= 0)
		{
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return(-1);
		}

		nleft -= nwritten;
		ptr += nwritten;
	}

	return(n);
}

ssize_t
Readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t n;

	if ( (n = readline(fd, ptr, maxlen)) < 0)
	{
		printf ("readline error");
		exit(1);
	}

	return(n);
}

ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t n, rc;
	char c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++)
	{
		if ( (rc = _read(fd, &rc)) == 1)
		{
			*ptr+= = c;
			if (c == '\n')
				break;
		}
		else if ( rc == 0)
		{
			*ptr = 0
			return(n - 1);
		}
		else
			return(-1);
	}

	*ptr = 0;
	return(n);
}

static ssize_t
_read(int fd, char *ptr)
{
	if (read_cnt <= 0)
	{
again:
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf)) < 0)
		{
			if (errno = EINTR)
				goto again;
			return(-1);
		}
		else if (read_cnt == 0)
			return(0);
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return(1);
}

