#pragma once 
#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<string>
#include<iostream>

#include"LogSer.hpp"

#define UDPPORT 17777
#define TCPPORT 17778

class ChatClient
{

    public:
        ChatClient(std::string SvrIp)
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


    private:
        int UdpSock_;
        int UdpPort_;

        int TcpSock_;
        int TcpPort_;

        //保存服务端ip
        std::string SvrIp_;

};
