//
//  wrap.c
//
//
//  Created by Victoria Mengqi LIU on 4/1/16.
//
//

#include	"ial.h"

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

int
Socket(int family, int type, int protocol)
{
	int n;
	
	if ( (n = socket(family, type, protocol)) < 0)
		printf("socket error");
	
	return(n);
}

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
		printf("bind error");
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
Sendto(int fd, const void *ptr, size_t nbytes, int flags, const struct sockaddr *sa, socklen_t salen)
{
	if (sendto(fd, ptr, nbytes, flags, sa, salen) != (ssize_t)nbytes)
		printf("sendto error");
}
