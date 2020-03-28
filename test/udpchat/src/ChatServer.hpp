#pragma once
#include"ConnectionInfo.hpp"
#include"LogSer.hpp"
#include"MsgPool.hpp"
#include<string>
#include<pthread.h>
#include<iostream>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<sys/types.h>

#define TCP_PROT 17778
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
            ,TcpSock_(-1)
            ,TcpPort_(TCP_PROT)
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
                LOG(FATAL, "Creat socket failed");
                exit(1);
            }
            //绑定地址信息
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(UdpPort_);
            //0.0.0.0 表示任意地址
            addr.sin_addr.s_addr = inet_addr("0.0.0.0");

            int ret = bind(UdpSock_, (struct sockaddr*)&addr,sizeof(addr));
            if(ret < 0)
            {
                LOG(FATAL, "bind addr failed");
                exit(2);
            }

            LOG(INFO, "Udp bind success");

            //初始化数据池
            MsgPool_ = new MsgPool();
            if(!MsgPool_)
            {
                LOG(FATAL,"Create MsgPool failed");
                exit(3);
            }
            LOG(INFO, "Create MsgPool success");

            //创建TCP-socket 
            TcpSock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if(TcpSock_ < 0)
            {
                LOG(FATAL, "Create tcp socket failed");
                exit(5);
            }

            struct sockaddr_in tcpaddr;
            tcpaddr.sin_family = AF_INET;
            tcpaddr.sin_port = htons(TcpPort_);
            tcpaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
            ret = bind(TcpSock_, (struct sockaddr*)&tcpaddr, sizeof(tcpaddr));
            if(ret < 0)
            {
                LOG(FATAL, "Bind TCP addrinfo failed");
                exit(6);
            }

            listen(TcpSock_, 5);
            if(ret < 0)
            {
                LOG(FATAL, "Tcp listen failed");
                exit(7);
            }

            LOG(INFO, "Tcp listen 0.0.0.0 : 17778");
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
                    LOG(FATAL,"pthread_create failed");
                    exit(4);
                }

                ret = pthread_create(&tid, NULL, ConsumeMsgStart, (void*)this);
                if(ret < 0)
                {
                    LOG(FATAL,"pthread_create failed");
                    exit(4);
                }
            }
            LOG(INFO, "UdpChat Service start success");

            while(1)
            {
                struct sockaddr_in cliaddr;
                socklen_t cliaddrlen = sizeof(cliaddr);
                int newsock = accept(TcpSock_,(struct sockaddr*)&cliaddr, &cliaddrlen);
                if(newsock < 0)
                {
                    LOG(ERROR, "Accep new connect failed");
                    continue;
                }

                LoginConnect* lc = new LoginConnect(newsock);
                if(!lc)
                {
                    LOG(ERROR, "Create LoginConnect failed");
                    continue;
                }
                //创建线程
                pthread_t tid;
                int ret = pthread_create(&tid, NULL, LoginReg, (void*)lc);
                if(ret < 0)
                {
                    LOG(ERROR, "Create User LoginConnect thread failed");
                    continue;
                }
                
                LOG(INFO, "Create TcpConnect thread start success");
            }

        }


    private:
        static void* LoginReg(void* arg)
        {
            pthread_detach(pthread_self());
            //登陆
            //获取cli端内容  recv(sock, buf, size, 0)
            //回复 send(sock, buf, size, 0)
            
            //注册
            return NULL;
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

        //tcp处理注册，登录请求
        int TcpSock_;
        int TcpPort_;

        void RevMsg()
        {
            char buff[10240] = {0};
            struct sockaddr_in cliaddr;
            socklen_t chiaddrlen = sizeof(struct sockaddr_in);
            int recvsize = recvfrom(UdpSock_, buff, sizeof(buff) - 1, 0, 
                    (struct sockaddr*)&cliaddr, &chiaddrlen);
            if(recvsize < 0)
            {
                LOG(ERROR,"recvfrom mag failed");
            }
            else 
            {
                std::string msg;
                msg.assign(buff, recvsize);
                LOG(INFO,msg);
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
                    LOG(ERROR,"sendto msg failed");
                }
                else 
                {
                    //成功
                }
        }
};
