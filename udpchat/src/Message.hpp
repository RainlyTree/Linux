#pragma once
#include <iostream>
#include "json/json.h"
#include <string>

class Message
{
    public:
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

        uint64_t& GetUserId()
        {
            return UserId_;
        }

    private:
        std::string NickName_;
        std::string School_;
        std::string Msg_;
        uint64_t UserId_;
};
