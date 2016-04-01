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
