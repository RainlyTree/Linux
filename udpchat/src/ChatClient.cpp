#include"ChatClient.hpp"
#include"ChatWindow.hpp"


void Menu()
{
    std::cout << "-------------------------" << std::endl;
    std::cout << "1.regeister       2.login" << std::endl;
    std::cout << "                         " << std::endl;
    std::cout << "3.logout          4.exit " << std::endl;
    std::cout << "-------------------------" << std::endl;
}

int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        printf("./Cli [ip]");
        exit(1);
    }

    

    ChatClient* cc = new ChatClient(argv[1]);
    //初始化服务
    cc->Init();
    while(1)
    {

        Menu();
        //选择 ----------->   注册     登陆
        int Select = -1;
        std::cout << "Please select service:";
        fflush(stdout);
        std::cin >> Select;
        if(Select == 1)
            {
            //注册
            if(!cc->Register())
            {
                std::cout << "Register failed" << std::endl;
            }
            else 
            {
                std::cout << "Register success!"<< std::endl;
            }
        }
        else if(Select == 2)
        {
            //登陆
            if(!cc->Login())
            {
                std::cout << "Login failed" << std::endl;
            }
            else 
            {
                std::cout << "Login success!" << std::endl;
                while(1)
                {
                    ChatWindow* cw = new ChatWindow();
                    cw->Start(cc);
                }
            }
        }
        else if(Select == 3)
        {
            //退出登录
            
        }
        else if(Select == 4)
        {
            //退出
            break;
        }
    }
    delete cc;
    return 0;
}
