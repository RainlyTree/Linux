#include<stdio.h>
#include<unistd.h>
#include<poll.h>

int main()
{
    struct pollfd fd_arr[10];
    fd_arr[0].fd = 0;
    fd_arr[0].events = POLLIN;


    //监控
    int ret = poll(fd_arr, 1, -1);
    if(ret < 0)
    {
        perror("poll");
        return 0;
    }

    //判断
    for(int i = 0; i < ret; ++i)
    {
        if(fd_arr[i].revents == POLLIN)
        {
            char buf[1024] = {0};
            read(fd_arr[i].fd, buf, sizeof(buf) - 1);
            printf("read content is [%s] \r\n", buf);
        }
    }
    return 0;
}
