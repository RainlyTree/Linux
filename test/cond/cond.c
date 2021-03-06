#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

int g_noodle = 0;

#define THREADCOUNT 4
pthread_mutex_t g_mutex;
pthread_cond_t g_eatcond;
pthread_cond_t g_makecond;

void* EatStart(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&g_mutex);
        while(g_noodle == 0)
        {
            pthread_cond_wait(&g_eatcond,&g_mutex);
        }
            --g_noodle;
        printf("eat is %d\n", g_noodle);
        pthread_mutex_unlock(&g_mutex);
        pthread_cond_signal(&g_makecond);
    }
    return NULL;
}

void* MakeStart(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&g_mutex);
        while(g_noodle == 1)
        {
            pthread_cond_wait(&g_makecond, &g_mutex);
        }
            ++g_noodle;
            printf("make is %d\n", g_noodle);
        pthread_mutex_unlock(&g_mutex);
        pthread_cond_signal(&g_eatcond);
    }
    return NULL;
}

int main()
{
    pthread_t eat_tid[THREADCOUNT],make_tid[THREADCOUNT];
    pthread_mutex_init(&g_mutex, NULL);
    pthread_cond_init(&g_eatcond, NULL);
    pthread_cond_init(&g_makecond, NULL);
    int i = 0;
    for(; i < THREADCOUNT; ++i)
    {
        int ret = pthread_create(&eat_tid[i], NULL, EatStart, NULL);
        if(ret != 0)
        {
            printf("creat thread fail");
            return 0;
        }
        ret = pthread_create(&eat_tid[i], NULL, MakeStart, NULL);
        if(ret != 0)
        {
            printf("creat thread fail");
            return 0;
        }
    }
    for(i = 0; i < THREADCOUNT; ++i)
    {
        pthread_join(eat_tid[i], NULL);
        pthread_join(make_tid[i], NULL);
    }
    pthread_mutex_destroy(&g_mutex);
    pthread_cond_destroy(&g_eatcond);
    pthread_cond_destroy(&g_makecond);
    return 0;
}
