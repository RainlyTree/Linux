#pragma once
#include"Message.hpp"
#include"UserManager.hpp"
#include"ConnectionInfo.hpp"
#include"LogSer.hpp"
#include"MsgPool.hpp"
#include"json/json.h"
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
                LOG(FATAL, "Creat socket failed") << std::endl;
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
                LOG(FATAL, "bind addr failed") << std::endl;
                exit(2);
            }

            LOG(INFO, "Udp bind success") << std::endl;

            //对用户管理初始化
            UserMana_ = new UserManager();
            if(!UserMana_)
            {
                LOG(FATAL, "Create Usermanager failed") << std::endl;
                exit(8);
            }

            //初始化数据池
            MsgPool_ = new MsgPool();
            if(!MsgPool_)
            {
                LOG(FATAL,"Create MsgPool failed") << std::endl;
                exit(3);
            }
            LOG(INFO, "Create MsgPool success") << std::endl;

            //创建TCP-socket 
            TcpSock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if(TcpSock_ < 0)
            {
                LOG(FATAL, "Create tcp socket failed") << std::endl;
                exit(5);
            }

            int opt = 1;
            setsockopt(TcpSock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            struct sockaddr_in tcpaddr;
            tcpaddr.sin_family = AF_INET;
            tcpaddr.sin_port = htons(TcpPort_);
            tcpaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
            ret = bind(TcpSock_, (struct sockaddr*)&tcpaddr, sizeof(tcpaddr));
            if(ret < 0)
            {
                LOG(FATAL, "Bind TCP addrinfo failed") << std::endl;
                exit(6);
            }

            ret = listen(TcpSock_, 5);
            if(ret < 0)
            {
                LOG(FATAL, "Tcp listen failed") << std::endl;
                exit(7);
            }

            LOG(INFO, "Tcp listen 0.0.0.0 : 17778") << std::endl;
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
                    LOG(FATAL,"pthread_create failed") <<std::endl;
                    exit(4);
                }

                ret = pthread_create(&tid, NULL, ConsumeMsgStart, (void*)this);
                if(ret < 0)
                {
                    LOG(FATAL,"pthread_create failed") << std::endl;
                    exit(4);
                }
            }
            LOG(INFO, "UdpChat Service start success") << std::endl;

            //处理登陆，注册请求
            while(1)
            {
                struct sockaddr_in cliaddr;
                socklen_t cliaddrlen = sizeof(cliaddr);
                int newsock = accept(TcpSock_,(struct sockaddr*)&cliaddr, &cliaddrlen);
                if(newsock < 0)
                {
                    LOG(ERROR, "Accep new connect failed") << std::endl;
                    continue;
                }

                //管理连接的类
                LoginConnect* lc = new LoginConnect(newsock, (void*)this);
                if(!lc)
                {
                    LOG(ERROR, "Create LoginConnect failed") << std::endl;
                    continue;
                }

                //创建线程
                pthread_t tid;
                int ret = pthread_create(&tid, NULL, LoginReg, (void*)lc);
                if(ret < 0)
                {
                    LOG(ERROR, "Create User LoginConnect thread failed") << std::endl;
                    continue;
                }
                
                LOG(INFO, "Create TcpConnect thread start success") << std::endl;
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
                LOG(ERROR, "Recv Tag type failed") << std::endl;
                return NULL;
            }
            else if(recvsize == 0)
            {
                LOG(ERROR, "Client shutdown connect") << std::endl;
                return NULL;
            }


            uint32_t UserId = -1;
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
                    UserStatus = REGFAILED;
                    LOG(ERROR, "Recv Request type not a effective failed") << std::endl;
                    break;
            }

            
            //服务端回复 send(sock, buf, size, 0)
            ReplyInfo ri;
            ri.Status = UserStatus;
            ri.UserId_ = UserId;
            ssize_t sendsize = send(lc->GetTcpSock(), &ri, sizeof(ri), 0);
            if(sendsize < 0)
            {
                //信息发送失败  是否需要考虑从新发送
                LOG(ERROR, "SendMsg Failed") << std::endl;
            }

            LOG(INFO, "Send success") << std::endl;
            //关闭连接
            close(lc->GetTcpSock());
            delete lc;
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

        int DealRegister(int sock,uint32_t* UserId)
        {
            //接收注册请求
            RegInfo ri;
            ssize_t recvsize = recv(sock, &ri, sizeof(ri), 0);
            if(recvsize < 0)
            {
                LOG(ERROR, "Recv TagType failed") << std::endl;
                return ONLINE;
            }
            else if(recvsize == 0)
            {
                LOG(ERROR, "Client shutdown connect")  << std::endl;
                //特殊处理对端关闭
            }
            //调用用户管理模块处理
            UserMana_->Register(ri.NiceName_,ri.School_, ri.Passwd_, UserId);
            //返回注册成功ID
            //返回状态
        }

        int DealLogin(int sock)
        {
            struct LoginInfo li;
            ssize_t recvsize = recv(sock, &li, sizeof(li), 0 );

            if(recvsize < 0)
            {
                LOG(ERROR, "Recv TagType failed") << std::endl;
                return OFFLINE;
            }
            else if(recvsize == 0)
            {
                LOG(ERROR, "Client shutdown connect") << std::endl;
            }

            LOG(DEBUG, "UserId:Passwd") << li.UserId_ << ":" << li.Passwd_ << std::endl;
            UserMana_->Login(li.UserId_, li.Passwd_);
        }

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
                LOG(ERROR,"recvfrom mag failed") << std::endl;
            }
            else 
            {
                //获取成功
                std::string msg;
                msg.assign(buff, recvsize);
                LOG(INFO,msg) << std::endl;
                //需要将发送的数据从JSON格式转化为可以识别的数据
                Message jsonmsg;
                jsonmsg.Deserialize(msg);
                //只有注册与登陆的人才能发送信息
                //校验当前消息为注册用户或老用户发送
                //判断是否是第一次发送消息 
                //是第一次则保存地址信息 更新状态为在线 将数据放入数据池
                bool ret = UserMana_->IsLogin(jsonmsg.GetUserId(), cliaddr, chiaddrlen);
                if(ret != true)
                {
                    LOG(ERROR, "discarded the msg") << msg << std::endl;
                }

                MsgPool_->PushMsgToPool(msg);
                LOG(INFO, "Push msg success") << std::endl;

            }

        }

        //广播接口
        void BroadcastMsg()
        {
            //获取发送内容
            std::string msg;
            MsgPool_->PopMsgFromPool(&msg);
            
            //获取要给哪个用户发送
            std::vector<UserInfo> OnlineUserVec;
            UserMana_->GetOnlineUserInfo(&OnlineUserVec);
            for(auto& e : OnlineUserVec)
            {
                //发送数据
                SendMsg(msg, e.GetCliAddrInfo(), e.GetCliAddrLen());;
            }
        }
        
        //消息发送个单个客户端
        void SendMsg(const std::string& msg, struct sockaddr_in& cliaddr,socklen_t& len)
        {
            ssize_t sendsize = 
                sendto(UdpSock_, msg.c_str(), msg.size(), 0, (struct sockaddr*)&cliaddr, len);
                if(sendsize < 0)
                {
                    //发送失败
                    LOG(ERROR,"sendto msg failed") << std::endl;
                    //是否缓存没有发送的信息和客户地址
                }
                else 
                {
                    //成功
                    LOG(INFO, "Sendto msg success") 
                        << "[" << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << "]" << msg << std::endl;
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
