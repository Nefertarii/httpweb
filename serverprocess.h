#ifndef SERVERPROCESS_H_
#define SERVERPROCESS_H_
#include "servhead.h"

class Server {
    private:
        int port, listenfd, epollfd;
        struct Clientinfo clients[MAX_CLI];
        void TCPlisten();
        void Createclient(int fd);
        void Socketclose(int epollfd,int fd);
        void Record();
        void Epollwrite(int fd);              //state ready to write
        void Epollread(int fd);                //state ready to read
        void Epolladd(int fd, void *cli);
        void Resetcache();

    public:
        Server(){};
        void Start(int epollfd);
        void Acceptconnect();
        void Socketwrite(std::string str, int length);
        void Socketwritefile(std::string filename);
        void Socketread(std::string *str);
        ~Server(){};
};
void Server::TCPlisten() {
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    Bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr));
    Listen(listenfd, LISTEN_WAIT);
}
void Server::Createclient(int fd) {
    struct tclient *client = nullptr;
    for (int i = 0; i < MAX_CLI; i++) {
        if (serverinfo.clients[i].fd < 0) {
            client = &serverinfo.clients[i];
            break;
        }
    }
    if (client) {
        client->fd = fd;
        Epolladd(serverinfo.epollfd, fd, client);
    }
    else {
        std::cout << "too many client: " << fd << std::endl;
        close(fd);
    }
}
void Server::Socketclose(int epollfd,int fd) {
    if (close(fd) < 0) {
        err_sys("close:");
    }
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr);
}
void Server::Record() {}
void Server::Epollwrite(int fd) {
    struct epoll_event ev;
    ev.events = EPOLLOUT | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}
void Server::Epollread(int fd) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}
void Server::Epolladd(int fd, void *cli) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = cli;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}
void Server::Resetcache(struct Cache *cache) {
    cache->send = 0;
    cache->remaining = 0;
    cache->filefd = 0;
    cache->str = "none";
}

void Server::Start(int port_) {
    epollfd = epoll_create(MAX_CLI);
    port = port_;
    TCPlisten(port);
    Epolladd(epollfd, fd, nullptr);
    std::cout << "Server start!" << std::endl;
}
void Server::Acceptconnect() {
    int connectfd = Accept(listenfd);
    Createclient(connectfd);
}
void Server::Socketwrite(std::string str, int length) {
    serverinfo.clientinfo[i].write.str = str;
    cache.remaining = length;
    int writesize = Write(sockfd, &cache);
    if (writesize < 0)
        Socketclose(SERV_EPOLL, serverinfo.CLI_FD);
    else if (writesize > MAX_BUF_SIZE) 
        std::cout << "kernel cache is full." << std::endl;
    else {
        Epollread(SERV_EPOLL, serverinfo.CLI_FD);
        Resetcache();
    }
}
void Server::Socketwritefile(std::string filename) {
    if (Readfile(filename, &cache) < 0) {
        //Httpsend(Not_Found);
        Socketclose(epollfd);
    }
    if (Writefile(sockfd, &cache) < 0) {
        Socketclose(epollfd);
    }
    Epollread(epollfd);
    Resetcache();
}
void Server::Socketread(std::string *str,struct Clientinfo *cli) {
    int readsize = Read(cli->fd, str);
    if (readsize <= 0)
        Socketclose(cli->fd);
    else { //读取信息处理 cli cache的大小也在此处设置
        Httpprocess(*str, cli);
        Epollwrite(cli->fd);
    }
}


#endif