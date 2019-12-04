#include<stdio.h>
#include<unistd.h>

int main()
{
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork");
        return 0;
    }
    else if(0 == pid)
    {
        printf("i am child %d\n",getpid());
    }
    else
    {
        printf("i am parent %d\n",getppid());
    }
    return 0;
}
