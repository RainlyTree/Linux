#pragma once 
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<string>
#include<iostream>

#include"LogSer.hpp"
#include"ConnectionInfo.hpp"

#define UDPPORT 17777
#define TCPPORT 17778

struct MySelf
{
    std::string NiceName_;
    std::string School_;
    std::string Passwd_;
    uint64_t UserId_;
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

            TcpSock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if(TcpSock_ < 0)
            {
                LOG(ERROR, "client create udp socket failed") << std::endl;
                exit(2);
            }
        }

        bool Connect2Server()
        {
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
            std::cout << "Please Enter Your NickName:";
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
                }
                else 
                {
                    printf("The password do not match twice\n");
                }
            }

            send_size = send(TcpPort_, &ri, sizeof(ri), 0);
            if(send_size < 0)
            {
                LOG(ERROR, "Send Register type failed") << std::endl;
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
                printf("注册成功, UserId = %lu\n", resp.UserId_);
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
            li.UserId_ = me_.UserId_;
            strcpy(li.Passwd_, me_.Passwd_.c_str());

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

        

    private:
        int UdpSock_;
        int UdpPort_;

        int TcpSock_;
        int TcpPort_;

        //保存服务端ip
        std::string SvrIp_;

        //客户端信息
        MySelf me_;
};
