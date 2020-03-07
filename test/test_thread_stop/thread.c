#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

void* thread_start(void* arg)
{
    printf("%d\n", pthread_self());
    while(1)
    {
        printf("i am a thread\n");
        printf("return NULL\n");
        sleep(1);
    }
    return NULL;
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, thread_start, NULL);
    printf("%d\n",tid);
    printf("%d\n", pthread_self());
    while(1)
    {
        printf("before\n");
        sleep(1);
    }
    return 0;
}
