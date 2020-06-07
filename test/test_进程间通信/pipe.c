#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

int main()
{
    int fd[2];
    int ret = pipe(fd);
    if(ret != 0)
    {
        perror("error");
        return 0;
    }

    //非阻塞设置
    int flag = fcntl(fd[1], F_GETFL, 0);
    flag |= O_NONBLOCK;
    fcntl(fd[1], F_SETFL, flag);

    int tmp = fork();
    if(tmp == 0)
    {
        //子进程
        sleep(1);
        write(fd[1], "test-pipe", 9);
        close(fd[0]);
        close(fd[1]);
    }

    else
    {
        close(fd[1]);

        char arr[1024] = {0};
        read(fd[0], arr, sizeof(arr) -1);

        printf("fd[0] read [%s]\n", arr);
        close(fd[0]);
    }
    return 0;
}
