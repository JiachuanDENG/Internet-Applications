//
// Read
//
// Author: Victoria Mengqi LIU
//
// Date: 15/06/16
//

#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

ssize_t
Read(int fildes, void *buf, size_t nbyte)
{
    int bytes_left = nbyte;
    int bytes_read = 0;
    char *ptr;

    while ( bytes_left > 0 ) {
        bytes_read = read(fildes, ptr, bytes_left);

        if ( bytes_read < 0 ) {
            if (errno == EINTR) 
                bytes_read = 0;
            else 
                return -1;
        } 
        else if ( bytes_read == 0 ) break;

        bytes_left -= bytes_read;
        ptr += bytes_read;
    }
    return (nbyte - bytes_left);
}
        


