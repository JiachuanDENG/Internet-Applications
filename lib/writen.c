/*
 * writen.c
 *
 * Author: Victoria Mengqi LIU
 * Language: C
 * Date: 29/4/16
 * Version 1.0
 */

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