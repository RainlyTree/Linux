#pragma once
#include <iostream>
#include "json/json.h"
#include <string>

class Message
{
    public:
        //序列化
        void Deserialize(std::string Message)
        {
            Json::Reader reader;
            Json::Value val;
            reader.parse(Message, val, false);

            NickName_ = val["NickName_"].asString();
            School_ = val["School_"].asString();
            Msg_ = val["Msg_"].asString();
            UserId_ = val["UserId_"].asInt();
        }


        void serialize(std::string* msg)
        {
            Json::Value val;
            val["NickName_"] = NickName_;
            val["School_"] = School_;
            val["Msg_"] = Msg_;
            val["UserId_"] = UserId_;

            Json::FastWriter writer;
            *msg = writer.write(val);

        }

        uint32_t& GetUserId()
        {
            return UserId_;
        }

        std::string& GetNickName()
        {
            return NickName_;
        }

        std::string& GetSchool()
        {
            return School_;
        }

        std::string& GetMsg()
        {
            return Msg_;
        }

        void SetNickName(std::string& NickName)
        {
            NickName_ = NickName;
        }

        void SetSchool(std::string& School)
        {
            School_ = School;
        }
        void SetMeg(std::string& Msg)
        {
            Msg_ = Msg;
        }
        void SetUserId(uint32_t& UserId)
        {
            UserId_ = UserId;
        }
    private:
        std::string NickName_;
        std::string School_;
        std::string Msg_;
        uint32_t UserId_;
};
