//
//  readline.c
//
//
//  Created by Victoria Mengqi LIU on 4/22/16.
//
//

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