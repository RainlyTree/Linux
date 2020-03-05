#include<stdio.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

void sigcalback(int sig)
{
    printf("i recv sig num is %d\n" ,sig);
    wait(NULL);
}

int main()
{
    signal(SIGCHLD,sigcalback);
    pid_t ret = fork();
    if(ret < 0)
    {
        perror("fork");
        return 0;
    }
    else if(ret == 0)
    {
        printf("child  exit \n");
        exit(1);
    }
    else
    {
        sleep(1);
        printf("father\n");
        while(1)
        {
            printf("no hear\n");
            sleep(2);
        }
    }
    return 0;
}
