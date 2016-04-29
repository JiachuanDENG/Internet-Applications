//
//  wrap.c
//
//
//  Created by Victoria Mengqi LIU on 4/1/16.
//
//



int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int		n;
    
again:
    if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef	EPROTO
        if (errno == EPROTO || errno == ECONNABORTED)
#else
            if (errno == ECONNABORTED)
#endif
                goto again;
            else
                printf("accept error");
    }
    return(n);
}

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if ( bind(fd, sa, salen) < 0) 
	{
		printf("bind error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
	}
}

void
Close(int fd)
{
	if (close(fd) == -1)
		printf("close error");
}

void
Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if (connect(fd, sa, salen) < 0)
	{
		printf("accept error: %s(errno: %d)", strerror(errno), errno);
        exit(0);
	}
}

char *
Fgets(char *ptr, int n, FILE *stream)
{
    char *rptr;
    
    if ( (rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
		printf("fgets error");
	
	return (rptr);
}

pid_t
Fork(void)
{
    pid_t pid;
    
    if ( (pid = fork()) == -1)
        printf("fork error");
    
    return (pid);
}

void
Fputs(const char *ptr, FILE *stream)
{
	if (fputs(ptr, stream) == EOF)
		printf("fputs error");
}

void
Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    if (getsockname(fd, sa, salenptr) < 0)
        printf("getsockname error");
}

const char *
Inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
	const char	*ptr;

	if (strptr == NULL)		/* check for old code */
		printf("NULL 3rd argument to inet_ntop");
	if ( (ptr = inet_ntop(family, addrptr, strptr, len)) == NULL)
		printf("inet_ntop error");		/* sets errno */
	return(ptr);
}

void
Inet_pton(int family, const char *strptr, void *addrptr)
{
	int		n;

	if ( (n = inet_pton(family, strptr, addrptr)) < 0)
		printf("inet_pton error for %s", strptr);	/* errno set */
	else if (n == 0)
		printf("inet_pton error for %s", strptr);	/* errno not set */

	/* nothing to return */
}

void
Listen(int fd, int backlog)
{
    char	*ptr;
    
    /*4can override 2nd argument with environment variable */
    if ( (ptr = getenv("LISTENQ")) != NULL)
        backlog = atoi(ptr);
    
    if (listen(fd, backlog) < 0)
	{
        printf("listen error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
    }
}

void *
Malloc(size_t size)
{
	void *ptr;

	if( (ptr = malloc(size)) == NULL)
		printf("malloc error");
	return(ptr);
}

ssize_t
Recv(int fd, void *ptr, size_t nbytes, int flags)
{
    ssize_t		n;
    
    if ( (n = recv(fd, ptr, nbytes, flags)) < 0)
        printf("recv error");
    return(n);
}

ssize_t
Recvfrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr *sa, socklen_t *salenptr)
{
	ssize_t n;

	if ( (n = recvfrom(fd, ptr, nbytes, flags, sa, salenptr)) < 0)
		printf("recvfrom error");
	
	return(n);
}

void
Send(int fd, const void *ptr, size_t nbytes, int flags)
{
    if (send(fd, ptr, nbytes, flags) != (ssize_t)nbytes)
        printf("send error");
}

void
Sendto(int fd, const void *ptr, size_t nbytes, int flags, const struct sockaddr *sa, socklen_t salen)
{
	if (sendto(fd, ptr, nbytes, flags, sa, salen) != (ssize_t)nbytes)
		printf("sendto error");
}

void
Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
    if (setsockopt(fd, level, optname, optval, optlen) < 0)
        printf("setsockopt error");
}

int
Socket(int family, int type, int protocol)
{
	int n;
	
	if ( (n = socket(family, type, protocol)) < 0)
	{
		printf("socket error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
	
	}
	
	return(n);
}

