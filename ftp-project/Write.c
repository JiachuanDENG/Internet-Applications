//
// Writen
//
// Author: Victoria Mengqi LIU
//
// Date: 15/06/16
//

#include <errno.h>
#include <unistd.h>

int
Write(int fildes, const void *buf, size_t nbyte)
{
    int bytes_left = nbyte;
    int bytes_written = 0;
    char *ptr = buf;

    while ( bytes_left > 0 ) {
        bytes_written = write(fildes, ptr, bytes_left);
        
        if ( bytes_written <= 0 ) {
            if ( errno == EINTR ) bytes_written = 0;
            else return -1;
        }

        bytes_left -= bytes_written;
        ptr += bytes_written;
    }

    return 0;
}
