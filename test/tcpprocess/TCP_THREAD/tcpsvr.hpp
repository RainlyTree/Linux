#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<iostream>

class TcpSvr
{
    public:
        TcpSvr()
            :Sockfd_(-1)
        {
        }

        ~TcpSvr()
        {
        }

        bool CreatSock()
        {
            Sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if(Sockfd_ < 0)
            {
                perror("socket\n");
            }
            return true;
        }

        bool Bind(std::string& ip, uint16_t port)
        {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = inet_addr(ip.c_str());
            int ret = bind(Sockfd_,(struct sockaddr*) &addr, sizeof(addr));
            if(ret < 0)
            {
                perror("bind");
                return false;
            }
            return true;
        }

        bool Listen(int BackLog = 5)
        {
            int ret = listen(Sockfd_, BackLog);
            if(ret < 0)
            {
                perror("listen");
                return false;
            }
            return true;
        }

        //连接接口，客户端---->服务端
        bool Connect(std::string& ip, uint16_t port)
        {
            struct sockaddr_in destaddr;
            destaddr.sin_family = AF_INET;
            destaddr.sin_port = htons(port);
            destaddr.sin_addr.s_addr = inet_addr(ip.c_str());
            int ret = connect(Sockfd_, (struct sockaddr*)&destaddr, sizeof(destaddr));
            if(ret < 0)
            {
                perror("connect");
                return false;
            }
            return true;
        }

        bool Accept(TcpSvr* ts, struct sockaddr_in* addr = NULL)
        {
            struct sockaddr_in peeraddr;
            socklen_t addrlen = sizeof(struct sockaddr_in);
            int NewSockFd = accept(Sockfd_, (struct sockaddr*)&peeraddr, & addrlen);
            if(NewSockFd < 0)
            {
                return false;
            }
            ts->Sockfd_ = NewSockFd;
            if(addr != NULL)
            {
                memcpy(addr, &peeraddr, addrlen);
            }
            return true;
        }

        bool Send(std::string& buf)
        {
            int ret = send(Sockfd_, buf.c_str(), buf.size(),0);
            if(ret < 0)
            {
                perror("send");
                return false;
            }
            return 0;
        }

        bool Recv(std::string& buffer)
        {
            char buf[1024] = {0};
            int ret = recv(Sockfd_, buf, sizeof(buf)- 1, 0);
            if(ret < 0)
            {
                perror("recv");
                return false;
            }
            else if(ret == 0)
            {
                //对端已经关闭
                printf("peer close this connect");
                return false;
            }
            buffer.assign(buf,ret);
            return true;
        }

        void Close()
        {
            close(Sockfd_);
            Sockfd_ = -1;
        }

    private:
        int Sockfd_;
};



//创建套接字
//绑定地址信息
//客户端---连接接口
//监听
//获取新连接
//发送数据
//接收数据
//关闭套接字
