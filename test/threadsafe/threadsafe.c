#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>

#define  THREADCOUNT 4
int g_tick = 100;
pthread_mutex_t mutex;

void* thread_start(void* arg)
{
    (void)arg;
    while(1)
    {
        pthread_mutex_lock(&mutex);
        if(g_tick > 0)
        {
            printf("i am thread [%d],i get ticket [%d]\n", *(int*)arg, g_tick); 
            g_tick = g_tick - 1;
            sleep(1);
        }
        else
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main()
{
    pthread_t tid[THREADCOUNT];
    int i = 0;
    pthread_mutex_init(&mutex, NULL);
    for(; i < THREADCOUNT; ++i)
    {
        pthread_create(&tid[i], NULL, thread_start, (void*)&i);
    }

    for(i = 0; i < THREADCOUNT; ++i)
    {
        pthread_join(tid[i],NULL);

    }
    pthread_mutex_destroy(&mutex);
    return 0;
}
