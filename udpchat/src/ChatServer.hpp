#pragma once
#include"UserManager.hpp"
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

//服务端聊天类 :  使用UDP协议
//1.接收客户端数据
//2.发送数据消息给客户端
class ChatServer
{
    public:
        ChatServer()
            :UdpSock_(-1)
            ,UdpPort_(UDP_PORT)
            ,MsgPool_(nullptr)
            ,TcpSock_(-1)
            ,TcpPort_(TCP_PROT)
            ,UserMana_(nullptr)
        {
        }

        ~ChatServer()
        {
            if(MsgPool_)
            {
                delete MsgPool_;
                MsgPool_ = nullptr;
            }
            if(UserMana_)
            {
                delete UserMana_;
                UserMana_ = nullptr;
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

            //对用户管理初始化
            UserMana_ = new UserManager();
            if(!UserMana_)
            {
                LOG(FATAL, "Create Usermanager failed");
                exit(8);
            }

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

            ret = listen(TcpSock_, 5);
            if(ret < 0)
            {
                LOG(FATAL, "Tcp listen failed");
                exit(7);
            }

            LOG(INFO, "Tcp listen 0.0.0.0 : 17778");
        }

        //初始化程序的生产和消费线程  ----->  登陆与注册
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

            //处理登陆，注册请求
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

                //管理连接的类
                LoginConnect* lc = new LoginConnect(newsock, (void*)this);
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
        //处理注册与登陆请求
        static void* LoginReg(void* arg)
        {
            pthread_detach(pthread_self());
            LoginConnect* lc = (LoginConnect*)arg; 
            ChatServer* cs = (ChatServer*) lc->GetServer();
            //注册，登录请求
            //获取cli端内容  recv(sock, buf, size, 0)
            char LoginType;
            ssize_t recvsize = recv(lc->GetTcpSock(), &LoginType, 1, 0);
            if(recvsize < 0)
            {
                LOG(ERROR, "Recv Tag type failed");
                return NULL;
            }
            else if(recvsize == 0)
            {
                LOG(ERROR, "Client shutdown connect");
                return NULL;
            }


            int UserId = -1;
            int UserStatus = -1;

            //收到正常请求标识
            switch(LoginType)
            {
                case REGISTER:
                    //使用用户管理模块注册
                    UserStatus =  cs->DealRegister(lc->GetTcpSock(), &UserId); 
                    break;
                case LOGIN:
                    //使用用户管理模块登陆
                    UserStatus = cs->DealLogin(lc->GetTcpSock());
                    break;
                case LOGINOUT:
                    //使用用户管理模块退出
                    UserStatus = cs->DealLoginOut();
                    break;
                default:
                    LOG(ERROR, "Recv Request type not a effective failed");
                    break;
            }
            //服务端回复 send(sock, buf, size, 0)
            
            //需要和用于模块进行打交道
            //注册
            return NULL;
        }
        static void* ProductMsgStart(void* arg)
        {
            //线程分离
            pthread_detach(pthread_self());
            
            ChatServer* cs = (ChatServer*)arg;
            while(1)
            {
                //接收信息
                cs->RevMsg();
            }
            return NULL;
        }


        static void* ConsumeMsgStart(void* arg)
        {
            //线程分离
            pthread_detach(pthread_self());

            ChatServer* cs = (ChatServer*)arg;
            while(1)
            {
                //广播信息
                cs->BroadcastMsg();
                //
            }
            return NULL;
        }

        int DealRegister(int sock,int* UserId)
        {
            //接收注册请求
            RegInfo ri;
            ssize_t recvsize = recv(sock, &ri, sizeof(ri), 0);
            if(recvsize < 0)
            {
                LOG(ERROR, "Recv TagType failed");
                return ONLINE;
            }
            else if(recvsize == 0)
            {
                LOG(ERROR, "Client shutdown connect");
                //特殊处理对端关闭
            }
            //调用用户管理模块处理
            UserMana_->Register();
            //返回注册成功ID
            //返回状态
        }

        int DealLogin(int sock)
        {}

        int DealLoginOut()
        {}
    private:

        void RevMsg()
        {
            char buff[10240] = {0};
            //对方地址信息
            struct sockaddr_in cliaddr;
            socklen_t chiaddrlen = sizeof(struct sockaddr_in);

            int recvsize = recvfrom(UdpSock_, buff, sizeof(buff) - 1, 0, 
                    (struct sockaddr*)&cliaddr, &chiaddrlen);
            if(recvsize < 0)
            {
                //获取失败
                LOG(ERROR,"recvfrom mag failed");
            }
            else 
            {
                //获取成功
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
            

            //获取发送内容
            std::string msg;
            MsgPool_->PopMsgFromPool(&msg);
        }
        
        //消息发送个单个客户端
        void SendMsg(const std::string& msg, struct sockaddr_in& cliaddr,socklen_t& len)
        {
            ssize_t sendsize = 
                sendto(UdpSock_, msg.c_str(), msg.size(), 0, (struct sockaddr*)&cliaddr, len);
                if(sendsize < 0)
                {
                    //发送失败
                    LOG(ERROR,"sendto msg failed");
                }
                else 
                {
                    //成功
                }
        }

    private:
        int UdpSock_;
        int UdpPort_;

        //数据池
        MsgPool* MsgPool_;

        //tcp处理注册，登录请求
        int TcpSock_;
        int TcpPort_;

        //用户管理
        UserManager* UserMana_;
};
