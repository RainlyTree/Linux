#pragma once 
#include"LogSer.hpp"

#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<vector>
#include<unordered_map>
#include<cstring>
#include<arpa/inet.h>
#include<iostream>
#include<string>

#define OFFLINE     0
#define REGISTERED  1
#define USERLOGINED 2
#define ONLINE      3 


class UserInfo
{
    public:
        //注册和登陆使用TCP 不应保存 应该等到第一次使用UDP发送消息时保存UDP
        UserInfo(const std::string& NickName, const std::string& School, 
                const uint32_t UserId, const std::string& Passwd)
            :NickName_(NickName)
            ,School_(School)
            ,UserId_(UserId)
            ,Passwd_(Passwd)
            ,CliAddrlen_(-1)
            ,UserStatus_(OFFLINE)
        {
            memset(&CliAddr_, '0', sizeof(struct sockaddr_in));
        }

        void SetUserStatus(int Status)
        {
            UserStatus_ = Status;
        }


        std::string& GetPasswd()
        {
            return Passwd_;
        }

        int& GetUserStatus()
        {
            return UserStatus_;
        }

        void SetCliAddr(const struct sockaddr_in& CliAddrInfo)
        {
            memcpy(&CliAddr_, &CliAddrInfo, sizeof(CliAddrInfo));
        }

        void SetCliAddrLen(const socklen_t& CliAddrlen)
        {
            CliAddrlen_ = CliAddrlen;
        }

        struct sockaddr_in& GetCliAddrInfo()
        {
            return CliAddr_;
        }

        socklen_t& GetCliAddrLen()
        {
            return CliAddrlen_;
        }

    private:
        std::string NickName_;
        std::string School_;
        //用户ID
        uint32_t UserId_;
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
            :PerpareUserId_(0)
        {
            OnlineUserVec_.clear();
            UserMap_.clear();
            pthread_mutex_init(&Lock_, NULL);
        }

        ~UserManager()
        {
            pthread_mutex_destroy(&Lock_);
        }

        int Register(const std::string& NickName, const std::string& School, const std::string& Passwd,uint32_t* UserId)
        {
            if(NickName.size() == 0 || School.size() == 0 || Passwd.size() == 0)
                return -1;

            pthread_mutex_lock(&Lock_);
            UserInfo userinfo(NickName, School,  PerpareUserId_, Passwd);
            //更改当前用户的状态,改为已注册状态
            userinfo.SetUserStatus(REGISTERED);
            //插入到map中
            UserMap_.insert(std::make_pair(PerpareUserId_, userinfo));
            *UserId = PerpareUserId_;
            //预ID增加
            ++PerpareUserId_;
            pthread_mutex_unlock(&Lock_);
            return 0;

        }

        int Login(const uint32_t& UserId, const std::string& Passwd)
        {
            if(Passwd.size() < 0)
                return -1;

            //返回的登陆状态
            int LoginState = -1;
            //查找是否存在这样的id
            //1.不存在  
            //2.存在  密码是否正确
            pthread_mutex_lock(&Lock_);
            auto iter = UserMap_.find(UserId);
            if(iter != UserMap_.end())
            {
                //查找到 -----> 判断是密码
                if(Passwd == iter->second.GetPasswd())
                {
                    //密码正确
                    iter->second.GetUserStatus() = USERLOGINED;
                    LoginState = 0;
                }
                else 
                {
                    LOG(ERROR, "User Passwd is not correct passwd is:") << Passwd << std::endl;
                    LoginState = -1;
                    //密码错误
                }
            }
            else 
            {
                LOG(ERROR, "UserId not found UserId is") << Passwd << std::endl;
                LoginState = -1;
                //没有查找到
            }
            pthread_mutex_unlock(&Lock_);

            //返回登陆状态
            return LoginState;
        }
        
        int LOginOut();

        bool IsLogin(uint32_t UserId, const struct sockaddr_in& cliudp, const socklen_t& cliaddrlen)
        {
            if(sizeof(cliudp) < 0 || cliaddrlen < 0)
            {
                return false;
            }

            //校验是否存在
            pthread_mutex_lock(&Lock_);
            auto iter = UserMap_.find(UserId);
            if(iter == UserMap_.end())
            {
                pthread_mutex_unlock(&Lock_);
                LOG(ERROR, "User not exit") << std::endl;
                return false;
            }

            //判断状态是否完成注册和登陆
            if(iter->second.GetUserStatus() == OFFLINE || iter->second.GetUserStatus() == REGISTERED)
            {
                pthread_mutex_unlock(&Lock_);
                LOG(ERROR, "User Status error") << std::endl;
                return false;
            }

            //判断当前用户是否是第一次发送信息
            if(iter->second.GetUserStatus() == ONLINE)
            {
                pthread_mutex_unlock(&Lock_);
                return true;
            }

            //第一次发送消息
            if(iter->second.GetUserStatus() == USERLOGINED)
            {
                iter->second.SetCliAddr(cliudp);
                iter->second.SetCliAddrLen(cliaddrlen);
                iter->second.SetUserStatus(ONLINE);
                //插入到vector在线数组中
                OnlineUserVec_.push_back(iter->second);
            }
            pthread_mutex_unlock(&Lock_);
            return true;
        }


        void GetOnlineUserInfo(std::vector<UserInfo>* vec)
        {
            *vec = OnlineUserVec_;
        }

    private:
        //保存注册用户信息 ---tcp
        std::unordered_map<uint32_t, UserInfo> UserMap_;
        //需要进行保护
        pthread_mutex_t Lock_;
        //保存在线用户信息 ---udp
        std::vector<UserInfo> OnlineUserVec_;
        //预分配用户id
        uint32_t PerpareUserId_;
};
