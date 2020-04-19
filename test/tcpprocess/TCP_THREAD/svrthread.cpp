#include"tcpsvr.hpp"
#include<sys/wait.h>
#include<signal.h>

void* ThreadStart(void* arg)
{
    pthread_detach(pthread_self());
    TcpSvr* ts = (TcpSvr*)arg;
    while(1)
    {
        std::string buf;
        ts->Recv(buf);
        printf("recv cli say:%s\n", buf.c_str());

        printf("svr send say:");
        fflush(stdout);
        std::cin >> buf;

        ts->Send(buf);
    }
    delete ts;
    ts->Close();
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("./tcpprocess.cpp [ip] [port] \n");
        return 0;
    }

    std::string ip = argv[1];
    uint16_t port = atoi(argv[2]);

    TcpSvr ts;
    if(!ts.CreatSock())
        return 0;

    if(!ts.Bind(ip,port))
        return 0;

    if(!ts.Listen(5))
        return 0;


    //获取链接
    while(1)
    {
        TcpSvr* peerts = new TcpSvr();
        struct sockaddr_in peeraddr;
        if(!ts.Accept(peerts, &peeraddr))
            continue;
        printf("Have a new connection %s: %d \n", 
                inet_ntoa(peeraddr.sin_addr), ntohl(peeraddr.sin_port));

        pthread_t tid;
        int ret = pthread_create(&tid, NULL, ThreadStart, (void*)peerts);
        if(ret < 0)
        {
            perror("pthread_fail");
            exit(1);
        }

    }


    ts.Close();
    return 0;
}
