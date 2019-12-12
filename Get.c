#include<stdio.h>
int main(int argc, char* argv[], char* env[])
{
    //获取环境变量
    //argc : 代表命令行参数的个数
    //argv : 
    //env  : 某一个环境变量的值
    int i = 0; 
    for(i = 0; i <argc; ++i)
    {
        printf("%s", argv[i]);
    }
    for(i = 0; env[i]; ++i)
    {
        printf("%s\n", env[i]);
    }
    return 0;
}
