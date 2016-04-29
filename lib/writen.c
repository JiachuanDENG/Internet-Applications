/* include writen */
#include	"ial.h"

int writen(char* dir,char* data){
    
    FILE * f;
    f = fopen(dir, "w+");
    if(f == NULL)
        printf("Can't open file!\n");
    
    fputs(data, f);
    
    fclose(f);
    
    return 0;
}