/*
类名        单词首字母大写 多单词用下划线分割
函数        单词首字母大写 多单词不分割 
变量        字母均小写且不分割
宏定义      字母全部大写
*/



#include "serverprocess.h"
void start_server()
{
    class Server Server_GWC;
    Server_GWC.Start(SERV_PORT);
    struct epoll_event events[MAX_CLI];
    std::string readbuf;
    signal(SIGCHLD, SIG_IGN);
    for (;;) {
        int nfds = epoll_wait(Server_GWC.Epollfd(), events, MAXEVENTS, TIMEOUT);
        if (nfds < 0 && errno != EINTR) {
            err_sys("epoll_wait error:");
            return;
        }
        for (int i = 0; i < nfds; i++) {
            struct epoll_event ev = events[i];
            if(ev.data.ptr == nullptr) {
                Server_GWC.Acceptconnect();
            } 
            else {
                if (ev.events & EPOLLIN) {
                    std::cout << "read...\t";
                    Server_GWC.Socketread(&readbuf, ev.data.ptr);
                }
                else if (ev.events & EPOLLOUT) {
                    std::cout << "write...\t";
                    Server_GWC.Socketwrite(ev.data.ptr);
                }
            }
        }
    }
}
