#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <err.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
    char *ptr, **pptr;
    struct hostent *hptr;
    char str[32];
    char ipaddr[16];
    struct in_addr hipaddr;
    
    if (2 != argc)
    {
        printf("need more args.\n");
        exit(-2);
    }

    ptr = argv[1];
    printf("Input IP: %s\n", ptr);
    if (! inet_aton(ptr, &hipaddr))
    {
        printf("inet_aton error\n");
        return 1;
    }
    if ( (hptr = gethostbyaddr(&hipaddr, sizeof(struct in_addr), AF_INET) ) == NULL )
    {
        printf("gethostbyaddr error for addr: %s\n", ptr);
		return 1;
    }
    
    printf("official hostname: %s\n",hptr->h_name);

    for (pptr = hptr->h_aliases; *pptr != NULL; pptr++)
        printf("alias: %s\n",*pptr);
    
    switch (hptr->h_addrtype)
    {
        case AF_INET:
        case AF_INET6:
            pptr = hptr->h_addr_list;
            for(; *pptr != NULL; pptr++)
                printf("address: %d\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
            break;

        default:
            printf("unknown address type\n");
            break;
    }

    return 0;
}
