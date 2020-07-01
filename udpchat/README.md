#Linux通过udp实现群聊系统的功能

#使用TCP服务实现客户注册和登陆

#客户注册 register() 
     1.用户昵称  NickName_ 
     2.学校 School_
     3.密码 Passwd_
     使用UserInfo结构体 保存上述内容
       额外保存
        UserId
        用户状态
            int status
        客户端地址信息
            struct sockaddr_in cliaddr;
            socklen_t cliaddrlen
#服务器返回  
     4.登陆ID UserId
     使用 unordered_map<UserId , UserInfo>保存

#登陆  Login()
     1.用户ID + 密码
     2.服务器进行用户认证
#认证成功 
     3.返回登陆成功结果，刷新出聊天界面

cli -----> 注册标识 ----->  注册请求 -------> 服务端

cli -----> 登陆标识 ----->  登陆请求 -------> 服务端

cli -----> 退出登陆标识 -----> 退出登陆请求 -------> 服务端




#用户发送信息 ---------->   客户端接收信息 -------->  放入信息池
#从信息池获取信息 --------->  广播给每一个在线用户


#服务端:
    1.获取客户端发送信息
    2.定义生产现场和消费线程
    3.生成数据存在数据池
    4.实现用户注册登陆
    5.从数据池获取数据进行群发


#数据池
    线程安全队列 + 2个接口（pop + push）
        线程安全:
            同步:条件变量
            互斥:互斥锁
        队列:
            STL::queue
        PushMsgToQue()
        PopMsgFromQue()
