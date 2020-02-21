#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

int main()
{
    int fds[2];
    char buf[100];
    int len;
    
    if(pipe(fds) == -1)
    {
        perror("make pipe");
        return 0;
    }

    while(fgets(buf,100,stdin))
    {
        len = strlen(buf);

        if(write(fds[1], buf, len) != len)
        {
            perror("write pipe");
            break;
        }
        memset(buf, 0x00, sizeof(buf));

        if((len = read(fds[0],buf,100)) == -1)
        {
            perror("read from pipe");
            break;
        }

        if(write(1,buf,len) == len)
        {
            perror("write to stdout");
            break;
        }
    }
    return 0;
}
