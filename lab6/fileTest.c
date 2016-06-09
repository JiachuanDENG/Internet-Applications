#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#define MAXLINE 1000000

int main(int argc, char *argv[])
{
    const char * dir;
    char readf[MAXLINE], ch;
    
    dir = argv[2];
    
    FILE *file = fopen(dir, "rb");
    FILE *nf = fopen("dhcp.pcap", "ab");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }
    
    printf("fopen() success, please wait...\n");
    
    while ( (ch = fgetc(file)) != EOF ) {
        fputc(ch, nf);
//        printf("%c", ch);
    }
    
    printf("fgets() success\n");
    
    fclose(file);
    return 0;
}