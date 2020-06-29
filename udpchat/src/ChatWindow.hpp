#pragma once 
#include<stdio.h>
#include<unistd.h>
#include<ncurses.h>
#include<vector>
#include<pthread.h>
#include<stdlib.h>


#include"ChatClient.hpp"
#include"Message.hpp"

class ChatWindow;

class Param
{
    public:
        Param(ChatWindow* winp, int threadnum, ChatClient* chatcli)
            :winp_(winp)
            ,ThreadNumber_(threadnum)
            ,Chatcli_(chatcli)
        {}
    public:
        ChatWindow* winp_;
        int ThreadNumber_;

        ChatClient* Chatcli_;
};

class ChatWindow
{
    public:
        ChatWindow()
            :header_(nullptr)
            ,output_(nullptr)
            ,input_(nullptr)
            ,user_list_(nullptr)
        {
            threads_.clear();
            pthread_mutex_init(&lock_, NULL);
            //初始化屏幕
            initscr();
            //不显示光标
            curs_set(0);
        }

        ~ChatWindow()
        {
            if(header_)
                delwin(header_);
            if(output_)
                delwin(output_);
            if(user_list_)
                delwin(user_list_);
            if(input_)
                delwin(input_);
            endwin();
            pthread_mutex_destroy(&lock_);
        }

        void DrawHeader()
        {
            int lines = LINES / 5;
            int cols =  COLS;
            int start_y = 0;
            int start_x = 0;
            header_ = newwin(lines, cols, start_y, start_x);
            box(header_, 0 , 0);
            pthread_mutex_lock(&lock_);
            wrefresh(header_);
            pthread_mutex_unlock(&lock_);
        }

        void DrawOutput()
        {
            int lines = LINES * 3 / 5;
            int cols =  COLS * 3 / 4;
            int start_y = LINES / 5;
            int start_x = 0;
            output_ = newwin(lines, cols, start_y, start_x); 
            box(output_, 0 , 0);
            pthread_mutex_lock(&lock_);
            wrefresh(output_);

            pthread_mutex_unlock(&lock_);

        }

        void DrawUserList()
        {
            int lines = LINES * 3 / 5;
            int cols =  COLS / 4;
            int start_y = LINES / 5;
            int start_x = COLS * 3 / 4;
            user_list_ = newwin(lines, cols, start_y, start_x);
            box(user_list_, 0 , 0);
            pthread_mutex_lock(&lock_);
            wrefresh(user_list_);
            pthread_mutex_unlock(&lock_);

        }

        void DrawInput()
        {
            int lines = LINES / 5;
            int cols =  COLS;
            int start_y = LINES *4 / 5;
            int start_x = 0;
            input_ = newwin(lines, cols, start_y, start_x);
            box(input_, 0 , 0);
            pthread_mutex_lock(&lock_);
            wrefresh(input_);
            pthread_mutex_unlock(&lock_);

        }


        //y 对应 行
        //x 对应 列
        void PutStringToWin(WINDOW* win, int y, int x, const std::string& msg)
        {
            //mvwaddstr 将字符放入窗口函数
            mvwaddstr(win , y, x, msg.c_str());
            pthread_mutex_lock(&lock_);
            wrefresh(win);
            pthread_mutex_unlock(&lock_);
        }

        void GetStringFromWin(WINDOW* win, std::string* Data)
        {
            char buf[1024] = {0};
            memset(buf, '\0', sizeof(buf));
            wgetnstr(win, buf, sizeof(buf) - 1);
            (*Data).assign(buf, strlen(buf));
        }

        static void RunHeader(ChatWindow* cw)
        {
            //绘制窗口
            int y, x;
            unsigned int  pos = 1;
            //标识是否需要改变输出的方向
            int flag = 0;
            std::string msg = "welcome to use is project";
            while(1)
            {
                cw->DrawHeader();
                //getmaxyx 返回窗口最大的行数存储在y当中，最大列存储在x当中
                getmaxyx(cw->header_, y, x);
                //欢迎语
                cw->PutStringToWin(cw->header_, y / 2, pos , msg);
                //处理左边边界
                if(pos < 2)
                {
                    flag = 0;
                }
                else if(pos > x - msg.size() - 2)
                {
                    flag = 1;
                }


                if(flag == 0)
                {
                    pos++;
                }
                else 
                {
                    pos--;
                }


                sleep(1);
            }
        }

        static void RunOutput(ChatWindow* cw, ChatClient* cc)
        {
            std::string recv_msg;
            Message msg;
            cw->DrawOutput();
            int line = 1;
            int y, x;
            while(1)
            {
                getmaxyx(cw->output_, y, x);
                cc->RecvMsg(&recv_msg);
                //反序列化
                msg.Deserialize(recv_msg);
                //展示数据
                std::string show_msg;
                show_msg += msg.GetNickName() + "-";
                show_msg += msg.GetSchool() + ":";
                show_msg += msg.GetMsg();
                if(line > y - 2)
                {
                    line = 1;
                    cw->DrawOutput();
                }

                cw->PutStringToWin(cw->output_, line, 1, show_msg);
                ++line;

                std::string user_info;
                user_info += msg.GetNickName() + "-";
                user_info += msg.GetSchool() ;

                cc->PushUser(user_info);
            }

        }

        static void RunIntput(ChatWindow* cw, ChatClient* cc)
        {
            //用户输入的原始信息
            std::string send_msg;
            //名称  学校 msg 用户id
            Message msg;
            msg.SetNickName(cc->GetMySelf().NiceName_);
            msg.SetSchool(cc->GetMySelf().School_);
            msg.SetUserId(cc->GetMySelf().UserId_);
            std::string tips = "please Enter# ";
            
            while(1)
            {
                cw->DrawInput();
                cw->PutStringToWin(cw->input_, 2, 2, tips);
                cw->GetStringFromWin(cw->input_, &send_msg);
                msg.SetMeg(send_msg);


                msg.serialize(&send_msg);

                cc->SendMsg(send_msg);

                sleep(1);
            }
        }

        static void RunUserList(ChatWindow* cw, ChatClient* cc)
        {
            int y, x;
            while(1)
            {
                cw->DrawUserList();
                getmaxyx(cw->user_list_, y, x);
                std::vector<std::string> UserList = cc->GetOnlieUser();
                for(auto& e : UserList)
                {
                    int line = 1;
                    cw->PutStringToWin(cw->user_list_, 1, line++, e);
                }
                sleep(1);
            }
        }
        static void* DrawWindow(void* arg)
        {
            Param* pm = (Param*)arg; 
            ChatWindow* cw = pm->winp_;
            ChatClient* cc = pm->Chatcli_;
            int thread_num = pm->ThreadNumber_;
            switch(thread_num)
            {
                case 0:
                    RunHeader(cw);
                    break;
                case 1:
                    RunOutput(cw, cc);
                    break;
                case 2:
                    RunUserList(cw, cc);
                    break;
                case 3:
                    RunIntput(cw, cc);
                    break;
                default:
                    break;
            }
            delete pm;
            return nullptr;
        }


        void Start(ChatClient* chatcli)
        {
            int i = 0;
            pthread_t tid;
            for(; i < 4; ++i)
            {
                Param* pm = new Param(this, i, chatcli);
                int ret = pthread_create(&tid, NULL, DrawWindow, (void*)pm);
                if(ret < 0)
                {
                    printf("Create thread failed\n");
                    exit(1);
                }
                threads_.push_back(tid);
            }

            for(i = 0; i < 4; ++i)
            {
                pthread_join(threads_[i], NULL);
            }
        }
    private:
        WINDOW* header_;
        WINDOW* output_;
        WINDOW* input_;
        WINDOW* user_list_;

        std::vector<pthread_t> threads_;
        pthread_mutex_t lock_;
};
