#include"tcpsvr.hpp"

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("./svr [ip][port]\n");
        return 0;
    }
    std::string ip = argv[1];
    uint16_t port = atoi(argv[2]);
    
    TcpSvr ts;
    if(!ts.CreatSock())
    {
        return 0;
    }

    if(!ts.Bind(ip,port))
    {
        return 0;
    }
    if(!ts.Listen())
    {
        return 0;
    }
    TcpSvr peerts;
        struct sockaddr_in peeraddr;
        if(!ts.Accept(peerts,&peeraddr))
        {
            return 0;
        }  
        printf("svr have a new connect, ip : port ----> %s : %d\n" , 
                inet_ntoa(peeraddr.sin_addr), 
                ntohs( peeraddr.sin_port));
    while(1)
    {
        std::string buf;
        peerts.Recv(buf);
        printf("client say : %s\n", buf.c_str());
    
        printf("server say:");
        fflush(stdout);
        std::cin >> buf;
        peerts.Send(buf);
    }

    peerts.Close();
    ts.Close();
    return 0;
}

