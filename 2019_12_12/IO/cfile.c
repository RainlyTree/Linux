#include<stdio.h>

int main()
{
    FILE* fd = fopen("tmp.txt", "r+");
    if(!fd)
    {
        perror("fopen");
        return -1;
    }
    printf("open success \n");
    return 0;
}
