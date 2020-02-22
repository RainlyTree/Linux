#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main()
{
    int fd = open("./a.txt", O_RDWR);
    if(fd < 0)
    {
        perror("open");
        return 0;
    }
    char buf[1024] = {0};

    read(fd, buf, sizeof(buf) - 1);
    printf(" read %s \n", buf);
    return 0;
}
