#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#define MAXLINE 1000000

int main(int argc, char *argv[])
{
    const char * dir;
    char buf[MAXLINE], ch;
    
    dir = argv[1];
    
    FILE *file = fopen(dir, "rb");
    FILE *nf = fopen("dhcp", "ab");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }
    
    printf("fopen() success, please wait...\n");
//    
//    while ( (ch = fgetc(file)) != EOF ) {
//        fputc(ch, nf);
////        printf("%c", ch);
//    }
//    
    while ( fgets(buf, MAXLINE-1, file) != NULL ) {
        fputs(buf, nf);
    }
    
    printf("fgets() success\n");
    
    fclose(nf);
    fclose(file);
    return 0;
}