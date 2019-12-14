#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc)
{
    printf("61");
    _exit(55);
    exit(66);
    if(argc != 3)
        return 88;
    int i = 0;
    for(i = 0; i < 256; i++)
    {
        printf("[%d]:[%s]\n", i, strerror(i));
    }
    return 0;
}
