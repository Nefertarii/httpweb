/*
����        ��������ĸ��д �൥�����»��߷ָ�
����        ��������ĸ��д �൥�ʲ��ָ� 
����        ��ĸ��Сд�Ҳ��ָ�
�궨��      ��ĸȫ����д
*/



#include "connectporcess.h"
void start_server()
{
    Server GWC("80");
    int epollfd = epoll_create(MAX_CLI);
    struct epoll_event ev;
    struct epoll_event events[MAXCLI];
    int listenfd = GWC.start(&epollfd);
    signal(SIGCHLD, SIG_IGN);
    for (;;) {
        int nfds = epollwait(epollfd, events, MAXEVENTS, TIMEOUT);
        if (nfds <= 0) {
            err_sys("epollwait error:")
        }
        for (int i = 0; i < nfds; i++) {
            GWC.Getepollevents(events[i]);
            if(events[i].data.fd == listenfd) {
                GWC.Acceptconnect();
            }
            else if (events[i].event & EPOLLIN) {
                GWC.Read();
            }
            else if (events[i].event & EPOLLOUT) {
                GWC.Write();
                GWC.Writefile();
            }
        }
    }
}
