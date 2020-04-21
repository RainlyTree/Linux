#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>

//创建套接字
//绑定地址信息
//客户端--连接接口
//监听
//获取新连接
//发送数据
//接收数据
//关闭套接字

class TcpSvr
{
    public:
        TcpSvr()
        {
            Sockfd_ = -1;
        }

        ~TcpSvr()
        {

        }

        bool CreateSock()
        {
            Sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if(Sockfd_ < 0)
            {
                perror("socket");
                return false;
            }
            return true;
        }

        bool Bind(std::string& ip, uint16_t port)
        {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = inet_addr(ip.c_str());
            int ret = bind(Sockfd_, (struct sockaddr*)&addr, sizeof(addr));
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

        //连接接口 是对 客户端而言的 客户端需要知道连接的服务端的ip地址和port信息
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

        //ts.send() ts.recv()
        bool Accept(TcpSvr& ts, struct sockaddr_in* addr = NULL)
        {
            struct sockaddr_in peeraddr;
            socklen_t addrlen = sizeof(struct sockaddr_in);
            int NewSockFd = accept(Sockfd_, (struct sockaddr*)&peeraddr, &addrlen);
            if(NewSockFd < 0)
            {
                return false;
            }
            ts.Sockfd_ = NewSockFd;

            if(addr != NULL)
            {
                memcpy(addr, &peeraddr, addrlen);
            }
            return true;
        }

        bool Send(std::string& buf)
        {
            int ret = send(Sockfd_, buf.c_str(), buf.size(), 0);
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
            int ret = recv(Sockfd_, buf, sizeof(buf) - 1, 0);
            if(ret < 0)
            {
                perror("recv");
                return false;
            }
            else if(ret == 0)
            {
                //对端将连接关闭了
                printf("peer close this connect\n");
                return false;
            }
            buffer.assign(buf, ret);
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


