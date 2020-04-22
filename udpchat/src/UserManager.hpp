#pragma once 
#include<vector>
#include<unordered_map>
#include<cstring>
#include<arpa/inet.h>
#include<iostream>
#include<string>


//状态
enum
{
    //未在线
    OFFLINE = 0,
    //已经注册完成
    REGISTERED,
    //登陆完成
    LOGINED,
    //在线
    ONLINE,
};

class UserInfo
{
    public:
        //注册和登陆使用TCP 不应保存 应该等到第一次使用UDP发送消息时保存UDP
        UserInfo(std::string& NickName, std::string& School, 
                uint64_t UserId, std::string& Passwd)
            :NickName_(NickName)
            ,School_(School)
            ,UserId_(UserId)
            ,Passwd_(Passwd)
            ,CliAddrlen_(-1)
            ,UserStatus_(OFFLINE)
        {
            memset(&CliAddr_, '0', sizeof(struct sockaddr_in));
        }
    private:
        std::string NickName_;
        std::string School_;
        //用户ID
        uint64_t UserId_;
        std::string Passwd_;
        //保存 udp客户端地址信息
        struct sockaddr_in CliAddr_;
        socklen_t CliAddrlen_;

        //保存当前用户状态
        
        int UserStatus_;

};


class UserManager
{
    public:
        UserManager()
        {
            OnlineUserVec_.clear();
            UserMap_.clear();
            pthread_mutex_init(&Lock_, NULL);
        }

        ~UserManager()
        {
            pthread_mutex_destroy(&Lock_);
        }

        int Register();

        int Login();
        
        int LOginOut();
    private:
        //保存注册用户信息
        std::unordered_map<uint64_t, UserInfo> UserMap_;
        //需要进行保护
        pthread_mutex_t Lock_;
        //保存在线用户信息
        std::vector<UserInfo> OnlineUserVec_;
};
