#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

//面条数量
int g_noodle = 0;

#define THREADCOUT 4

pthread_mutex_t g_mutex;

void* MakeStart(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&g_mutex);
        printf("make g_noodle %d\n",g_noodle);
        g_noodle++;
        pthread_mutex_unlock(&g_mutex);
    }
    return NULL;
}

void* EatStart(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&g_mutex);

        printf("eat g_noodle %d\n",g_noodle);
        g_noodle--;
        pthread_mutex_unlock(&g_mutex);
    }
    return NULL;
}
int main()
{

    pthread_mutex_init(&g_mutex,NULL);

    pthread_t eat_tid[THREADCOUT], make_tid[THREADCOUT];
    int i = 0;
    for(; i < THREADCOUT; i++)
    {
        int ret = pthread_create(&eat_tid[i], NULL, EatStart, NULL);
        if(ret != 0)
        {
            printf("creat failed\n");
            return 0;
        }
        ret = pthread_create(&make_tid[i],NULL, MakeStart, NULL);
        if(ret != 0)
        {
            printf("creat failed\n");
            return 0;
        }
    }
    for(i = 0; i < THREADCOUT; i++)
    {
        pthread_join(eat_tid[i], NULL);
        pthread_join(make_tid[i], NULL);
    }
    pthread_mutex_destroy(&g_mutex);
    return 0;
}
