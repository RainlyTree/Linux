#include<stdio.h>
#include<unistd.h>
#include<signal.h>
void sigcalback(int sig)
{
    printf("sig : %d\n", sig);
}

int main()
{
    signal(2,sigcalback);
    

    sigset_t set, oldset;
    sigemptyset(&set);
    sigemptyset(&oldset);

    //全部置1
    sigfillset(&set);

    sigprocmask(SIG_BLOCK, &set, &oldset);
    printf("test, signal\n");
    getchar();
    sigprocmask(SIG_UNBLOCK, &set, NULL);

    while(1)
    {
        printf("test signal\n");
        sleep(1);
    }
    return 0;
}
