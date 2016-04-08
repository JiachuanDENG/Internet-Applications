//
//  ial.h
//
//
//  Created by Victoria Mengqi LIU on 4/1/16.
//
//

#include <stdio.h>		/* for printf() and fprintf() */
#include <stdlib.h>		/* for atoi() and exit() */
#include <string.h>		/* for memset() */

#include <errno.h>
#include <netdb.h>		/*  */
#include <unistd.h>		/* for close() */

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>	/* for socket(), sendto() and recvfrom() */

#include <arpa/inet.h>	/* for sockaddr_in and inet_Addr() */


#define ECHOMAX 255		/* Longest string to echo */

#define	LISTENQ		1024

/* Miscellaneouis constants */
#define MAXLINE 4096	/* max text line length */
#define BUFFSIZE 8192 	/* buffer size for reads and writes */

/* Define some port number that will be used in I-A-Lab */
#define SERV_PORT 9877	/* TCP and UDP */
#define SER_PORT_STR "9877"		/* TCP and UDP */

/* Following shortens all the typecasts of pointer arguments: */
#define SA struct sockaddr


