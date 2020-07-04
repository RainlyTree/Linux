#pragma once 
#include<stdio.h>
#include<unistd.h>
#include<sys/select.h>
#include<vector>

#include "tcpsvr.hpp"

class SelectSvr
{
    public:
        SelectSvr() 
        {
            FD_ZERO(&readfds_);
            max_fd_ = -1;
        }

        void AddFd(int fd)
        {
            FD_SET(fd, &readfds_);
            if(fd > max_fd_)
            {
                max_fd_ = fd;
            }
        }

        void DeleteFd(int fd)
        {
            FD_CLR(fd, &readfds_);
        }

        bool SelectWait(std::vector<TcpSvr>* out)
        {
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 3000;
            fd_set tmp = readfds_;
            int ret = select(max_fd_ + 1, &tmp, NULL, NULL, &tv);
            if(ret < 0)
            {
                perror("select");
                return false;
            }
            else if(ret == 0)
            {
                return false;
            }

            for(int i = 0; i <= max_fd_; ++i)
            {
                if(FD_ISSET(i, &tmp))
                {
                    TcpSvr ts;
                    ts.Setfd(i);
                    out->push_back(ts);
                }
            }
            return true;
        }
    private:
        int max_fd_;
        fd_set readfds_;
};
