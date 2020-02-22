#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

int main()
{
    int fd = open("./a.txt", O_RDWR);
    if(fd < 0)
    {
        perror("open");
        return 0;
    }
    write(fd, "test-mkfifo", 11);
    return 0;
}
