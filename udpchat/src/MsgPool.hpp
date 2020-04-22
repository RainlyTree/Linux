#pragma once 
#include<pthread.h>
#include<queue>
#include<iostream>
#include<string>

//防止线程无止境输入
#define CAPACITY 1024

class MsgPool
{
    public:
        MsgPool()
            :capacity_(CAPACITY)
        {
            pthread_mutex_init(&MsgQueLock_, NULL);
            pthread_cond_init(&SynComQue_, NULL);
            pthread_cond_init(&SynProQue_, NULL);
        }

        ~MsgPool()
        {
            pthread_mutex_destroy(&MsgQueLock_);
            pthread_cond_destroy(&SynComQue_);
            pthread_cond_destroy(&SynProQue_);
        }


        void PushMsgToPool(std::string& msg)
        {
            pthread_mutex_lock(&MsgQueLock_);
            while(IsFull())
            {
                pthread_cond_wait(&SynProQue_, &MsgQueLock_);
            }
            MsgQue_.push(msg);
            pthread_mutex_unlock(&MsgQueLock_);
            pthread_cond_signal(&SynComQue_);
        }

        //msg为出参
        void PopMsgFromPool(std::string* msg)
        {
            pthread_mutex_lock(&MsgQueLock_);
            while(MsgQue_.empty())
            {
                pthread_cond_wait(&SynComQue_, &MsgQueLock_);
            }
            *msg = MsgQue_.front();
            MsgQue_.pop();
            pthread_mutex_unlock(&MsgQueLock_);
            pthread_cond_signal(&SynProQue_);
        }

    private:

        bool IsFull()
        {
            if(MsgQue_.size() == capacity_)
            {
                return true;
            }
            return false;
        }

    private:

        std::queue<std::string> MsgQue_;

        //约束队列的大小
        size_t capacity_;
        //互斥锁
        pthread_mutex_t MsgQueLock_;

        //保证进程的同步
        //消费者条件变量
        pthread_cond_t SynComQue_;
        //生产者条件变量
        pthread_cond_t SynProQue_;

};
