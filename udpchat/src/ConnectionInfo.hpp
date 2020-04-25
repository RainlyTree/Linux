#pragma once 
#include<iostream>

#define REGISTER 0
#define LOGIN 1
#define LOGINOUT 2


//约定双方发送的数据包最大多大
#define MESSAGE_MAX_SIZE 1024

//注册信息
struct RegInfo
{
    char NiceName_[15];
    char School_[20];
    char Passwd_[20];
};

//登录信息
struct LoginInfo
{
    uint32_t UserId_;
    char Passwd_[20];
};

enum UserStatus
{
    //注册失败
    REGFAILED = 0,
    //成功REGISTERED
    REGISTERED,
    LOGINFAILED,
    LOGINED
};

//应答信息
struct ReplyInfo
{
    //当前状态
    int Status;
    uint32_t UserId_;
};


class LoginConnect
{
    public:
        LoginConnect(int sock, void* server)
            :Sock_(sock)
            ,Server_(server)
        {
        }

        int GetTcpSock()
        {
            return Sock_;
        }

        void* GetServer()
        {
            return Server_;
        }


    private:
        int Sock_;
        //用户保存ChatServer类的实例化指针
        void* Server_;
};
