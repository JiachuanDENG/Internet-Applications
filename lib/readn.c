/*
 * readn.c
 *
 * Author: Victoria Mengqi LIU
 * Language: C
 * Date: 29/4/16
 * Version 1.0
 */

#include "ial.h"

char* readn(char* dir)
{
    FILE * f;
    f = fopen(dir, "r");
    
    if(f == NULL)
        printf("Can't open file!\n");
    
    char s[100];
    
    fgets(s, 100, f);
    fclose(f);
    
    return s;
}