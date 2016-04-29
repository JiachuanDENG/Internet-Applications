#include "../lib/ial.h"

ssize_t
writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *prt;

	ptr = vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ( (nwritten = write(fd, ptr, nleft)) <= 0)
		{
			if (nwriteem < 0 && errno == EINTR)
				nwritten = 0;
			else
				return(-1);
		}

		n_left -= nwritten;
		ptr += written;
	}

	return(n);
}

void
Writen(int fd, void *ptr, size_t nbytes)
{
	if ( writen(fd, ptr, nbytes) != nbytes)
		print ("writen error");
}
