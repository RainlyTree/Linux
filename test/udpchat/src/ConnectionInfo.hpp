#pragma once 
#include<iostream>

//注册信息
struct RegInfo
{
    char Name_[15];
    char School_[20];
    char Passwd_[20];
};

//登录信息
struct LoginInfo
{
    uint64_t UserId_;
    char Passwd_[20];
};

//应答信息
struct ReplyInfo
{
    int Status;
    uint64_t UserId_;
};


class LoginConnect
{
    public:
        LoginConnect(int sock)
            :Sock_(sock)
        {
        }
    private:
        int Sock_;
};
