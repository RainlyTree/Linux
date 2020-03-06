#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

void* thread_start(void* arg)
{
    while(1)
    {

    printf("i am new thread  -  %d\n", (int*)arg);
    sleep(1);
    }
    return NULL;

    }

int main()
{
    pthread_t tid;
    char arr[1024];
    sprintf(arr,"%s" , "Linux");
    int i = 0;
    for(; i < 4; ++i)
    {

    pthread_create(&tid, NULL, thread_start, (void*)i);
    }

    while(1)
    {
        printf("i am best thread\n");
        sleep(1);
    }
    return 0;
}
