#pragma once
#include"LogSer.hpp"
#include"MsgPool.hpp"
#include<string>
#include<pthread.h>
#include<iostream>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/types.h>
#define UDP_PORT 17777
#define THREAD_COUNT 2
//服务端聊天
//1.接收客户端数据
//2.发送数据消息给客户端
//使用UDP协议
class ChatServer
{
    public:
        ChatServer()
            :UdpSock_(-1)
            ,UdpPort_(UDP_PORT)
            ,MsgPool_(nullptr)
        {
        }

        ~ChatServer()
        {
            if(MsgPool_)
            {
                delete MsgPool_;
                MsgPool_ = nullptr;
            }
        }

        //调用InitServer函数初始化UDP
        void InitServer()
        {
            //创建套接字
            UdpSock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if(UdpSock_ < 0)
            {
                perror("socket");
                exit(1);
            }
            //绑定地址信息
            struct sockaddr_in addr;
            addr.sin_family = AF_FILE;
            addr.sin_port = htons(UdpPort_);
            //0.0.0.0 表示任意地址
            addr.sin_addr.s_addr = inet_addr("0.0.0.0");

            int ret = bind(UdpSock_, (struct sockaddr*)&addr,sizeof(addr));
            if(ret < 0)
            {
                perror("bind addr failed");
                exit(2);
            }


            //初始化数据池
            MsgPool_ = new MsgPool();
            if(!MsgPool_)
            {
                perror("Create MsgPool failed");
                exit(3);
            }
        }

        //初始化程序的生产和消费线程
        void Start()
        {
            pthread_t tid;
            for(int i = 0; i < THREAD_COUNT; ++i)
            {
                int ret = pthread_create(&tid, NULL, ProductMsgStart, (void*)this);
                if(ret < 0)
                {
                    perror("pthread_create failed");
                    exit(4);
                }

                ret = pthread_create(&tid, NULL, ConsumeMsgStart, (void*)this);
                if(ret < 0)
                {
                    perror("pthread_create failed");
                    exit(4);
                }
            }
        }


        static void* ProductMsgStart(void* arg)
        {
            pthread_detach(pthread_self());
            ChatServer* cs = (ChatServer*)arg;
            while(1)
            {
                //recvfrom
                cs->RevMsg();
            }
            return NULL;
        }


        static void* ConsumeMsgStart(void* arg)
        {
            pthread_detach(pthread_self());
            ChatServer* cs = (ChatServer*)arg;
            while(1)
            {
                cs->BroadcastMsg();
                //
            }
            return NULL;
        }
    private:
        int UdpSock_;
        int UdpPort_;

        MsgPool* MsgPool_;

        void RevMsg()
        {
            char buff[10240] = {0};
            struct sockaddr_in cliaddr;
            socklen_t chiaddrlen = sizeof(struct sockaddr_in);
            int recvsize = recvfrom(UdpSock_, buff, sizeof(buff) - 1, 0, 
                    (struct sockaddr*)&cliaddr, &chiaddrlen);
            if(recvsize < 0)
            {
                perror("recvfrom mag failed");
            }
            else 
            {
                std::string msg;
                msg.assign(buff, recvsize);
                MsgPool_->PushMsgToPool(msg);
            }

        }

        //广播接口
        void BroadcastMsg()
        {
            //获取要给哪个用户发送
            std::string msg;
            MsgPool_->PopMsgFromPool(&msg);
            //获取发送内容
        }
        
        //给一个客户端发送的接口
        void SendMsg(const std::string& msg, struct sockaddr_in& cliaddr,
                socklen_t& len)
        {
            ssize_t sendsize = sendto(UdpSock_, msg.c_str(), msg.size(), 0, (struct sockaddr*)&cliaddr, len);
                if(sendsize < 0)
                {
                    perror("sendto msg failed");
                }
                else 
                {
                    //成功
                }
        }
};
