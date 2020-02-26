#include<stdio.h>
#include<sys/unistd.h>

int main()
{
    while(1)
    {
        printf("i am waiting sigal\n");
        sleep(1);
    }
    return 0;
}
