#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>

#define  THREADCOUNT 4
int g_tick = 100;

void* thread_start(void* arg)
{
    (void)arg;
    while(1)
    {
        if(g_tick > 0)
        {
            g_tick = g_tick - 1;
            printf("i am thread [%d],i get ticket [%d]\n", pthread_self(), g_tick);
            
            usleep(1000);
        }
        else
        {
            break;
        }
    }
    return NULL;
}

int main()
{
    pthread_t tid[THREADCOUNT];
    int i = 0;
    for(; i < THREADCOUNT; ++i)
    {
        pthread_create(&tid[i], NULL, thread_start, NULL);
    }

    for(i = 0; i < THREADCOUNT; ++i)
    {
        pthread_join(tid[i],NULL);

    }
    return 0;
}
