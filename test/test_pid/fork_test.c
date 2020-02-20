#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main()
{
    int ret = fork();
    if(ret < 0)
    {
        perror("fork");
        return 1;
    }
    else if (ret == 0)
    {
        printf("i am child :%d, ret : %d\n",getpid(), ret);
    }
    else
    {
        printf("i am father :%d, ret : %d\n",getpid(), ret);
    }
    return 0;
}
