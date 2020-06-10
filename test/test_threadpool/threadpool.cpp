#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<iostream>
#include<queue>
#include<stdlib.h>

#define  THREADCOUNT 4

//封装任务类

void ThreadTask2(int Data)
{
    printf("i recv Data [%d]\n", Data);
}

void ThreadTask1(int Data)
{
    Data += 10;
    printf("i recv Data [%d]\n", Data);
}

typedef void (*Handler_t)(int);

class ThreadTask
{
    public:
        ThreadTask()
            :Data_(-1)
            ,Handler_(NULL)
        {}
        ThreadTask(int Data, Handler_t Handler)
            :Data_(Data)
            ,Handler_(Handler)
        {}

        void run()
        {
            Handler_(Data_);
        }


    private:
        int Data_;
        Handler_t Handler_;
};

class ThreadPool
{
    public:
        ThreadPool()
            :Capacity_(10)
            ,ThreadCapacity_(THREADCOUNT)
            ,ThreadCurNum_(THREADCOUNT)
            ,IsExit_(false)
        {
            pthread_mutex_init(&Mutex_, NULL);
            pthread_cond_init(&Cond_, NULL);

            //创建线程
            bool IsCreate = ThreadCreate();
            if(!IsCreate)
            {
                printf("ThreadPool Create failed\n");
                exit(1);
            }
        }

        ~ThreadPool()
        {
            pthread_mutex_destroy(&Mutex_);
            pthread_cond_destroy(&Cond_);
        }

        bool Push(ThreadTask* Tt)
        {
            pthread_mutex_lock(&Mutex_);
            if(IsExit_)
            {
                pthread_mutex_unlock(&Mutex_);
                return false;
            }
            Que_.push(Tt);
            pthread_mutex_unlock(&Mutex_);
            //放入队列后通知线程池当中线程
            pthread_cond_signal(&Cond_);
            return true;
        }

        bool Pop(ThreadTask** Tt)
        {
            *Tt = Que_.front();
            Que_.pop();
            return true;
        }

        void ThreadPoolClear()
        {
            pthread_mutex_lock(&Mutex_);
            IsExit_ = true;
            pthread_mutex_unlock(&Mutex_);

            while(ThreadCurNum_ > 0)
                pthread_cond_broadcast(&Cond_);
        }
        
        void ThreadExit()
        {
            for(int i = 0; i < THREADCOUNT; ++i)
            {
                pthread_cancel(tid_[i]);
            }
        }

        void ThreadJoin()
        {
            for(int i = 0; i < THREADCOUNT; ++i)
            {
                pthread_join(tid_[i] , NULL);
            }
        }

        static void* ThreadStart(void* arg)
        {
            ThreadPool* tp = (ThreadPool*)arg;
            while(1)
            { 
                pthread_mutex_lock(&tp->Mutex_);
                while(tp->Que_.empty())
                {
                    if(tp->IsExit_)
                    {
                        tp->ThreadQuit();
                    }
                    
                    pthread_cond_wait(&tp->Cond_, &tp->Mutex_);
                }
                ThreadTask* tt;
                tp->Pop(&tt);
                pthread_mutex_unlock(&tp->Mutex_);
                tt->run();

                delete tt;
            }
            //出队处理数据的操作
            return NULL;
        }

        bool ThreadCreate()
        {
            int ret;
            for(int i = 0; i < THREADCOUNT; ++i)
            {
                ret = pthread_create(&tid_[i], NULL, ThreadStart, (void*)this);
                if(ret != 0)
                {
                    perror("pthread_create fail");
                    return false;
                }
            }
            return true;
        }

        void ThreadQuit()
        {
            --ThreadCurNum_;
            pthread_mutex_unlock(&Mutex_);
            pthread_exit(NULL);
        }

    private:
        std::queue<ThreadTask*> Que_;
        size_t Capacity_;
        
        //线程池
        //线程
        pthread_t tid_[THREADCOUNT];
        //线程池容量
        size_t ThreadCapacity_;
        //当前线程池有多少线程
        size_t ThreadCurNum_;

        //线程安全机制
        pthread_mutex_t Mutex_;
        pthread_cond_t Cond_;
        //线程安全退出标志
        bool IsExit_;
};

int main()
{
    ThreadPool* tp = new ThreadPool();
    for(int i = 0; i < 10; ++i)
    {
        ThreadTask* tt = new ThreadTask(i, ThreadTask2);
        tp->Push(tt);
    }

    sleep(6);
    tp->ThreadPoolClear();
    tp->ThreadJoin();

    delete tp;
    return 0;
}
