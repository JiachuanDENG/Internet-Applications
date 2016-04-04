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

#include <netdb.h>		/*  */
#include <unistd.h>		/* for close() */

#include <sys/types.h>
#include <sys/socket.h>	/* for socket(), sendto() and recvfrom() */

#include <arpa/inet.h>	/* for sockaddr_in and inet_Addr() */


#define ECHOMAX 255		/* Longest string to echo */
