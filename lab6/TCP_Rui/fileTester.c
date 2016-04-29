#include<stdio.h>

int main(int argc, char const *argv[]) {

    FILE * f;
    f = fopen("/Users/Rui/Desktop/tcp/temp.txt", "r");
    if(f == NULL)
        printf("Can't open file!\n");

    char s[100];

    fgets(s, 100, f);
    printf("%s\n", s);

    fclose(f);
    return 0;
}
