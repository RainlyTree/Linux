#pragma once 
#include<cstring>
#include<cstdio>
#include<stdio.h>
#include<sys/time.h>
#include<iostream>

//日志格式   ----- > [时间 info/warning/error/fatal/debug 文件  行号] 具体错误信息

const char* Level[] = 
{
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL",
    "DEBUG"
};


enum LogLevel
{
    INFO = 0,
    WARNING,
    ERROR,
    FATAL,
    DEBUG
};

//获取时间
class LogTime
{
    public:
        static int64_t GetTimeStamp()
        {
            //两个参数   struct timeval （秒，毫秒）  2.时区  一般传NULL
            struct timeval tv;
            gettimeofday(&tv,NULL);
            return tv.tv_sec;
        }

        static void GetTimeStamp1(std::string& timestamp)
        {
            //返回 年月日 时分秒
            time_t SysTime;
            time(&SysTime);

            struct tm* ST = localtime(&SysTime);
            //格式化字符串
            char TimeNow[23] = {'\0'};
            snprintf(TimeNow, sizeof(TimeNow) - 1, "%04d-%02d-%02d-%02d:%02d:%02d",
                    ST->tm_year + 1900, ST->tm_mon, ST->tm_mday, ST->tm_hour,
                    ST->tm_min, ST->tm_sec);
            timestamp.assign(TimeNow, strlen(TimeNow));
        }
};

inline void Log(LogLevel lev, const char* file, int line, std::string& logmsg)
{
    std::string level_info = Level[lev];
    std::string timer_stamp;
    
    LogTime::GetTimeStamp1(timer_stamp);

    std::cout << "[" << timer_stamp << " " << level_info << " " << file <<
        ":" << line << "]" << logmsg << std::endl;
}
