/*
����        ��������ĸ��д �൥�����»��߷ָ�
����        ��������ĸ��д �൥�ʲ��ָ� 
����        ��ĸ��Сд�Ҳ��ָ�
�궨��      ��ĸȫ����д
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
