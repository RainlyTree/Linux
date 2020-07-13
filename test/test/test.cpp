#include<stdio.h>
#include<random>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<iostream>
using namespace std;

int fi = 0;
bool fia = false;
int se = 0;
bool fis = false;
pthread_mutex_t Lock;
pthread_cond_t Rand_cond;
pthread_cond_t Rand1_cond;
pthread_cond_t Sum_cond;

void* Rand(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&Lock);
        while(fia)
        {
            pthread_cond_wait(&Rand_cond, &Lock);
        }
        random_device e;
        fi = e();
        cout << "fin = " << fi << endl; 
        fia = true;
        pthread_mutex_unlock(&Lock);
        pthread_cond_signal(&Rand1_cond);
    }
}


void* Rand1(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&Lock);
        while(!fia || fis)
        {
            pthread_cond_wait(&Rand1_cond, &Lock);
        }
        random_device e;
        se = e();
        cout << "fis = " << se << endl;
        fis = true;
        pthread_mutex_unlock(&Lock);
        pthread_cond_signal(&Sum_cond);
    }
}

void* Sum(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&Lock);
        while(!fis && !fia)
        {
            pthread_cond_wait(&Sum_cond, &Lock);
        }
        int sum = fi + se;
        fis = false;
        fia = false;
        cout << sum << endl;
        sleep(1);
        pthread_mutex_unlock(&Lock);
        pthread_cond_signal(&Rand_cond);
    }
}

int main()
{
    pthread_cond_init(&Rand_cond, NULL);
    pthread_cond_init(&Rand1_cond, NULL);
    pthread_cond_init(&Sum_cond, NULL);
    pthread_mutex_init(&Lock, NULL);
    pthread_t arr[3];
    int ret = pthread_create(&arr[0], NULL, Rand, NULL);
    if(ret != 0)
    {
        cout << "create faile" << endl;
        return 0;
    }
    ret = pthread_create(&arr[1], NULL, Rand1, NULL);
    if(ret != 0)
    {
        cout << "create faile" << endl;
        return 0;
    }
    ret = pthread_create(&arr[2], NULL, Sum, NULL);
    if(ret != 0)
    {
        cout << "create faile" << endl;
        return 0;
    }

    pthread_join(arr[0], NULL);
    pthread_join(arr[1], NULL);
    pthread_join(arr[2], NULL);
    pthread_mutex_destroy(&Lock);
    pthread_cond_destroy(&Rand_cond);
    pthread_cond_destroy(&Rand1_cond);
    pthread_cond_destroy(&Sum_cond);
    return 0;
}
