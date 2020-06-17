#pragma once
#include<vector>
#include<stdio.h>
#include<unistd.h>
#include<sys/select.h>
#include"tcpsvr.hpp"

class SelectSvr
{
    public:
        SelectSvr()
        {
            FD_ZERO(&readfds_);
            max_fd_ = -1;
        }

        bool AddFd(int fd)
        {
            FD_SET(fd, &readfds_);
            if(fd > max_fd_)
            {
                max_fd_ = fd;
                return true;
            }
            return true;
        }

        bool DeleteFd(int fd)
        {
            //删除文件描述符
            FD_CLR(fd, &readfds_);
            //更新最大文件描述符
            for(int i = max_fd_; i >= 0; --i)
            {
                if(!FD_ISSET(fd, &readfds_))
                {
                    continue;
                }
                max_fd_ = i;
                break;
            }
            return true;
        }

        bool SelectWait(std::vector<TcpSvr>* out)
        {
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 30000;
            fd_set tmp = readfds_;
            int ret = select(max_fd_ + 1, &tmp, NULL, NULL, &tv);
            if(ret < 0)
            {
                perror("select");
                return false;
            }
            else if(ret == 0)
            {
                printf("select timeout \n");
                return false;
            }

            //正常情况
            for(int i = 0; i <= max_fd_; ++i)
            {
                if(FD_ISSET(i, &tmp))
                {
                    //返回给上层调用
                    TcpSvr ts;
                    ts.Setfd(i);
                    out->push_back(ts);
                }
            }
            return true;

        }
    private:
        //最大文件描述符
        int max_fd_;
        fd_set readfds_;
};
