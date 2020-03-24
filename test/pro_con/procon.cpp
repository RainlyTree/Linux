#include<stdio.h>
#include<pthread.h>
#include<iostream>
#include<queue>
#define THREADCOUNT 10

class BlockQueue
{
    public:
        BlockQueue(int Capacity)
            :Capacity_(Capacity)
        {
            pthread_mutex_init(&QueueMutex_, NULL);
            pthread_cond_init(&ConsumeCond_, NULL);
            pthread_cond_init(&ProductCond_, NULL);
        }

        ~BlockQueue()
        {
            pthread_mutex_destroy(&QueueMutex_);
            pthread_cond_destroy(&ConsumeCond_);
            pthread_cond_destroy(&ProductCond_);
        }

        int Pop(int* Data)
        {
            pthread_mutex_lock(&QueueMutex_);
            while(Queue_.empty())
            {
                pthread_cond_wait(&ConsumeCond_, &QueueMutex_);
            }
            *Data = Queue_.front();
            Queue_.pop();
            pthread_mutex_unlock(&QueueMutex_);
            pthread_cond_signal(&ProductCond_);
            return 0;
        }


        int Push(int& Data)
        {
            pthread_mutex_lock(&QueueMutex_);
            while(IsFull())
            {
                pthread_cond_wait(&ProductCond_, &QueueMutex_);
            }
            Queue_.push(Data);
            pthread_mutex_unlock(&QueueMutex_);
            pthread_cond_signal(&ConsumeCond_);
            return 0;
        }



    private:
        std::queue<int> Queue_;
        size_t Capacity_;
        pthread_mutex_t QueueMutex_;
        pthread_cond_t ConsumeCond_;
        pthread_cond_t ProductCond_;


        bool IsFull()
        {
            return Queue_.size() == Capacity_;
        }
};


void* ConsumeStart(void* arg)
{
    BlockQueue* bq = (BlockQueue*)arg;
    while(1)
    {
        int Data;
        bq->Pop(&Data);
        printf("ConsumeStart [%lu] [%d] \n", pthread_self(), Data);
    }
    return NULL;
}

void* ProduceStart(void* arg)
{
    BlockQueue* bq = (BlockQueue*)arg;
    int i = 0;
    while(1)
    {
        bq->Push(i);
        printf("ProduceStart [%lu] [%d]\n", pthread_self(), i);
        ++i;
    }
    return NULL;
}



int main()
{
    pthread_t Con_tid[THREADCOUNT] , Pro_tid[THREADCOUNT];
    BlockQueue * bq = new BlockQueue(1);
    int i = 0;
    for(i = 0; i < THREADCOUNT; ++i)
    {
        int ret = pthread_create(&Con_tid[i], NULL, ConsumeStart, (void*)bq);
        if(ret != 0)
        {
            std::cout << "creat failed" << std::endl;
            return 0;
        }
        ret = pthread_create(&Pro_tid[i], NULL, ProduceStart, (void*)bq);
        if(ret != 0)
        {
            std::cout << "creat failed" << std::endl;
            return 0;
        }
    }


    for(i = 0; i < THREADCOUNT; ++i)
    {
        pthread_join(Con_tid[i], NULL);
        pthread_join(Pro_tid[i], NULL);
    }
    return 0;
}
