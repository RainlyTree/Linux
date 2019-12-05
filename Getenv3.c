#include<stdio.h>
#include<stdlib.h>

int main()
{
    //char* getenv(const char* name);
    printf("%s\n", getenv("PATH"));
    return 0;
}
