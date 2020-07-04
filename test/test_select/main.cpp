#include"tcpsvr.hpp"
#include"SelectSvr.hpp"

#define CHECK_PET(p) if(p != true){return -1;};

int main()
{
    TcpSvr listen_ts;
    CHECK_PET(listen_ts.CreateSock());
    CHECK_PET(listen_ts.Bind("192.168.79.128", 19999));
    CHECK_PET(listen_ts.Listen());

    SelectSvr ss;
    ss.AddFd(listen_ts.Getfd());

    while(1)
    {
        std::vector<TcpSvr> out;
        if(!ss.SelectWait(&out))
        {
            continue;
        }

        for(size_t i = 0; i < out.size(); ++i)
        {
            if(listen_ts.Getfd() == out[i].Getfd())
            {
                TcpSvr tmp;
                struct sockaddr_in peeraddr;
                listen_ts.Accept(tmp, &peeraddr);
                printf("new connection : %s-%d\n", inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
                ss.AddFd(tmp.Getfd());
            }
            else 
            {
                printf("\n");
                std::string buf;
                out[i].Recv(buf);
                printf("client send data is \"%s\" \n", buf.c_str());
            }
        }
    }
    return 0;
}
