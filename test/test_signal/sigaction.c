#include<stdio.h>
#include<unistd.h>
#include<signal.h>

void sigcallback(int signum)
{
    printf("signum - %d\n", signum);
}

int main()
{
    struct sigaction newact;
    struct sigaction oldact;

    newact.sa_handler = sigcallback;
    newact.sa_flags = 0;
    //清空位图中的比特位
    sigemptyset(&newact.sa_mask);
    sigaction(SIGINT, &newact, &oldact);

    while(1)
    {
        printf("test\n");
        sleep(1);
    }
    return 0;
}
