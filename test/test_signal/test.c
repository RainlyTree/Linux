#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main()
{
    char* lp = (char*)malloc(10);
    memset(lp, '\0', 10);
    strcpy(lp, "test");
    printf("%s\n", lp);
    free(lp);
    lp = NULL;
    return 0;
}
