#include<stdio.h>
#include<signal.h>
#include<string.h>
#include<stdlib.h>

//signal (int signum, sighander_t handler)

void sigcallback(int signum)
{
    printf("%d", signum);
}

int main()
{
   signal(2, sigcallback) ;

   while(1)
   {
       ;
   }
    return 0;
}
