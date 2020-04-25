#pragma once 
#include<stdio.h>
#include<unistd.h>
#include<ncurses.h>
#include<vector>
#include<pthread.h>
#include<stdlib.h>


#include"ChatClient.hpp"

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
            initscr();
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

        static void* DrawWindow(void* arg)
        {
            Param* pm = (Param*)arg; 
            ChatWindow* cw = pm->winp_;
            ChatClient* cc = pm->Chatcli_;
            int thread_num = pm->ThreadNumber_;
            switch(thread_num)
            {
                case 0:
                    cw->DrawHeader();
                    break;
                case 1:
                    cw->DrawOutput();
                    break;
                case 2:
                    cw->DrawUserList();
                    break;
                case 3:
                    cw->DrawInput();
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
        }
    private:
        WINDOW* header_;
        WINDOW* output_;
        WINDOW* input_;
        WINDOW* user_list_;

        std::vector<pthread_t> threads_;
        pthread_mutex_t lock_;
};
