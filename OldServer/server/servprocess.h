#ifndef SERVPROCESS_H_
#define SERVPROCESS_H_

#include "process.h"
#include <vector>
#include <signal.h>
#include <sys/epoll.h>

class Server
{
private:
    int listenfd, epollfd;
    std::vector<std::shared_ptr<Clientinfo>> clients;
    struct epoll_event ev, events[MAX_CLI];
public:
    Server();
    void Start();
    void Stop();
    void Record();
    void Acceptconnect();
    void TCPlisten(int port);
    void Createclient(int fd);
    void Closeclient(CLIENT *cli);
    void Epollwrite(CLIENT *cli); //ready to write
    void Epollread(CLIENT *cli);  //ready to read
    void Epolladd(int fd, CLIENT *cli);
    void BadRequest(int statecode, CLIENT *cli);
    void Socketwrite(void *cli);
    void Socketread(void *cli);
    ~Server(){};
};

#endif