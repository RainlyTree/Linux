#pragma once 
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<string>
#include<iostream>
#include<vector>

#include"LogSer.hpp"
#include"ConnectionInfo.hpp"

#define UDPPORT 17777
#define TCPPORT 17778

struct MySelf
{
    std::string NiceName_;
    std::string School_;
    std::string Passwd_;
    uint32_t UserId_;
};

class ChatClient
{

    public:
        ChatClient(std::string SvrIp = "192.168.79.128")
            :UdpSock_(-1)
            ,UdpPort_ (UDPPORT)
            ,TcpSock_(-1)
            ,TcpPort_(TCPPORT)
            ,SvrIp_(SvrIp)
        {
        }

        ~ChatClient()
        {
            if(UdpSock_ > 0)
            {
                close(UdpSock_);
            }
        }

        void Init()
        {
            UdpSock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if(UdpSock_ < 0)
            {
                LOG(ERROR, "client create udp socket failed") << std::endl;
                exit(1);
            }

        }

        bool Connect2Server()
        {
            //创建TCPsocket
            TcpSock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if(TcpSock_ < 0)
            {
                LOG(ERROR, "client create udp socket failed") << std::endl;
                exit(2);
            }
            //服务端 地址信息填充
            struct sockaddr_in peer;
            peer.sin_family = AF_INET;
            peer.sin_port = htons(TcpPort_);
            peer.sin_addr.s_addr = inet_addr(SvrIp_.c_str());

            int ret = connect(TcpSock_, (struct sockaddr*)&peer, sizeof(peer));
            if(ret < 0)
            {
                LOG(ERROR, "Connect Server failed") << SvrIp_ << ":" << TcpPort_ << std::endl;
                return false;
            }
            return true;
        }

        bool Register()
        {
            if(!Connect2Server())
            {
                return false;
            }
            //连接成功
            //发送注册标识
            char type = REGISTER;
            ssize_t send_size = send(TcpSock_, &type, 1, 0);
            if(send_size < 0)
            {
                LOG(ERROR, "Send Register type failed");
                return false;
            }
            //发送注册内容
            struct RegInfo ri;
            std::cout << "Please Enter Your NiceName:";
            std::cin >> ri.NiceName_;
            std::cout << "Please Enter Your School:";
            std::cin >> ri.School_;
            while(1)
            {

                std::cout << "Please Enter Your Passwd:";
                std::string PasswdOne;
                std::cin >> PasswdOne;
                std::cout << "Please Enter Your Password again:";
                std::string PasswdAgain;
                std::cin >> PasswdAgain;
                if(PasswdAgain == PasswdOne)
                {
                    strcpy(ri.Passwd_ , PasswdOne.c_str());
                    break;
                }
                else 
                {
                    printf("The password do not match twice\n");
                }
            }

            send_size = send(TcpSock_, &ri, sizeof(ri), 0);
            if(send_size < 0)
            {
                LOG(ERROR, "Send Register message failed") << std::endl;
                return false;
            }

            //获取用户ID
            struct ReplyInfo resp;
            ssize_t recv_size = recv(TcpSock_, &resp, sizeof(resp), 0);
            if(recv_size < 0)
            {
                LOG(ERROR, "recv register response failed") << std::endl;
                return false;
            }
            else if(recv_size == 0)
            {
                LOG(ERROR, "Peer shutdown connect") << std::endl;
                return false;
            }
            if(resp.Status == REGISTERED)
            {
                printf("注册成功, UserId = %u\n", resp.UserId_);
                //保存UserId
                me_.NiceName_ = ri.NiceName_;
                me_.School_ = ri.School_;
                me_.Passwd_ = ri.Passwd_;
                me_.UserId_ = resp.UserId_;
                LOG(INFO, "Register success") << std::endl;
                return true;
            }
            LOG(ERROR, "Register failed") << std::endl;
            printf("注册失败\n");
            close(TcpSock_);
            return false;
        }

        bool Login()
        {
            if(!Connect2Server())
            {
                return false;
            }

            //发送登陆标识
            char type = LOGIN;
            ssize_t send_size = send(TcpSock_, &type, 1, 0);
            if(send_size < 0)
            {
                LOG(ERROR, "Send Login type failed") << std::endl;
                return false;
            }
            //发送登陆数据
            struct LoginInfo li;
            std::cout << "UserId:";
            std::cin >> li.UserId_ ;
            std::cout << "Passwd:";
            std::cin >> li.Passwd_;
            me_.UserId_ = li.UserId_; 
            me_.Passwd_ = li.Passwd_;
            //li.UserId_ = me_.UserId_;
            //strcpy(li.Passwd_, me_.Passwd_.c_str());

            send_size = send(TcpSock_, &li, sizeof(li), 0);
            if(send_size < 0)
            {
                LOG(ERROR, "Send Login fata failed") << std::endl;
                return false;
            }
            //解析登陆状态
            struct ReplyInfo resp;
            ssize_t recv_size = recv(TcpSock_, &resp, sizeof(resp), 0);
            if(recv_size < 0)
            {
                LOG(ERROR,"Peer shutdown connect ") << std::endl;
                return false;
            }
            if(resp.Status != LOGINED)
            {
                LOG(ERROR, "Login Failed Status is ") << resp.Status<< std::endl;
                printf("登陆失败\n");
                return false;
            }
            LOG(INFO,"Login success") << std::endl;
            printf("登陆成功\n");
            return true;
        }

        
        //udp数据收发
        bool SendMsg(const std::string& msg)
        {
            struct sockaddr_in peer;
            peer.sin_family = AF_INET;
            peer.sin_port = htons(UdpPort_);
            peer.sin_addr.s_addr = inet_addr(SvrIp_.c_str());
            ssize_t send_size = sendto(UdpSock_, msg.c_str(), msg.size(), 0, (struct sockaddr*)&peer, sizeof(sockaddr));
            if(send_size < 0)
            {
                LOG(ERROR, "Send Msg to Server Failed\n");
                return false;
            }
            return true;
        }



        bool RecvMsg(std::string* msg)
        {
            char buf[MESSAGE_MAX_SIZE];
            memset(buf, '\0',sizeof(buf));
            struct sockaddr_in svraddr;
            socklen_t svraddrlen = sizeof(svraddr);
            ssize_t recv_size = recvfrom(UdpSock_, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&svraddr, &svraddrlen);
            if(recv_size < 0)
            {
                LOG(ERROR, "recv msg from server failed");
                return false;
            }
            (*msg).assign(buf, recv_size);
            return true;
        }

        MySelf& GetMySelf()
        {
            return me_;
        }

        void PushUser(std::string& user_info)
        {
            auto iter = OnlineUser.begin();
            while(iter != OnlineUser.end())
            {
                if(*iter == user_info)
                    return;
            }

            OnlineUser.push_back(user_info);
        }

        std::vector<std::string>& GetOnlieUser()
        {
            return OnlineUser;
        }

    private:
        int UdpSock_;
        int UdpPort_;

        int TcpSock_;
        int TcpPort_;

        //保存服务端ip
        std::string SvrIp_;

        //客户端信息
        MySelf me_;

        //保存在线用户
        std::vector<std::string> OnlineUser;
};
