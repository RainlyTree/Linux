#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
char g_Command[1024];

int GetCommand()
{
    //读取一行
    memset(g_Command, '\0', 1024);
    printf("[minishell@localhost]$ ");
    fflush(stdout);
    if(!fgets(g_Command, sizeof(g_Command) - 1, stdin))
    {
        printf("error");
        return -1;
    }

    printf("[%s]\n",g_Command);
    return 0;
}

int ExecCommand(char* argv[])
{
    if(argv[0] == NULL)
    {
        printf("ExecCommand error");
        return - 1;
    }
    pid_t pid = fork();
    if(pid < 0)
        return - 1;
    else if(pid == 0)
    {
        //child
        execvp(argv[0], argv);
        exit(0);
    }
    else
    {
        //father
        waitpid(pid, NULL, 0);
    }
    return 0;
}


int DealCommand(char* command)
{
    if(!command || *command == '\0')
    {
        printf("command error");
        return -1;
    }
   
    int argc = 0;
    char* argv[32] = { 0 };

    while(*command)
    {
        if(!isspace(*command))
        {
            argv[argc] = command;
            ++argc;
            while(!isspace(*command) && *command != '\0')
                ++command;
            *command = '\0';

        }
        ++command;
    }


    argv[argc] = NULL;
    int i;
    for(i = 0; i < argc; ++i)
    {
        printf("[%d][%s]\n", i, argv[i]);
    }

    ExecCommand(argv);
    return 0;
}



int main()
{
    while(1)
    {
        if(GetCommand() == -1)
            continue;
        //处理获得的命令
        DealCommand(g_Command);
    }

    return 0;
}
