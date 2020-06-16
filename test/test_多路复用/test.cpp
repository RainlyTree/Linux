#include<stdio.h>
#include<unistd.h>
#include<sys/select.h>
#include<fcntl.h>

int main()
{
    int fd = open("./tmpfile", O_RDWR | O_CREAT, 0664);
    if(fd < 0)
    {
        perror("open");
        return 0;
    }
    printf("file fd = %d \n" ,fd);
    fd_set readfds;
    FD_ZERO(&readfds);

    //添加
    FD_SET(fd, &readfds);
    //FD_SET(0, &readfds);
    //FD_SET(2, &readfds);

    //监控
    int ret = select(fd + 1, &readfds, NULL, NULL, NULL);
    if(ret)
    {
        printf("select() = ret : %d\n", ret);
        if(FD_ISSET(fd, &readfds))
        {
            ret = FD_ISSET(fd, &readfds);
            printf("FD_ISSET(fd, &readfds) ---- > ret %d\n", ret);
            char buf[1024] = {0};
            read(0, buf, sizeof(buf) - 1);
            printf("buf : %s \n", buf);
        }
    }

    //清楚
    FD_CLR(0, &readfds);
    return 0;
}
