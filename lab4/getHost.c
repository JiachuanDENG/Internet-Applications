//
//  getHost.c
//
//
//  Created by Victoria Mengqi LIU on 4/1/16.
//
//

#include "../lib/ial.h"
#include "../lib/wrap.c"

int main(int argc, char **argv)
{
	char *ptr, **pptr;
	char buf[8192];
    struct hostent *hptr;
	struct in_addr inaddr;

	while (--argc > 0)
	{
		ptr = *++argv;
		if ( (hptr = gethostbyname(ptr)) == NULL )
		{
			printf("gethostbyname error for host: %s: %s", 
					ptr, hstrerror(h_errno));
			continue;
		}
		printf("official hostname: %s\n", hptr->h_name);

		for (pptr = hptr->h_aliases; *pptr != NULL; pptr++)
			printf("\talias: %s\n", *pptr);

		switch (hptr->h_addrtype)
		{
			case AF_INET:
				pptr = hptr->h_addr_list;
				for ( ; *pptr != NULL; pptr++)
				{
					inaddr = *((struct in_addr *)(*pptr));
					printf("\tIPv4 address: %s", inet_ntoa(inaddr));

					if ( (hptr = gethostbyaddr((char *) &inaddr,
							sizeof(struct in_addr), AF_INET)) == NULL )
						printf("\t(gethostbyaddr failed: %s)\n",
								hstrerror(h_errno));
					else if (hptr->h_name != NULL)
						printf("\tname = %s\n", hptr->h_name);
					else
						printf("\t(no hostname returned by gethostbyaddr)\n");
				}
                break;

			default:
				printf("unknown address type");
				break;
		}
	}
	
	exit(0);
}
