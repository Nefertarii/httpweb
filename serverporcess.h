#ifndef SERVERPORCESS_H_
#define SERVERPORCESS_H_
#include "servhead.h"

class Server {
    private:
        int epollfd, listenfd;
        std::string port;
        struct epoll_event ev;
        struct Cache cache;
        int Socketclose(int sockfd);
        void Record();
        void Epollwrite(int sockfd);              //ready to write
        void Epollread(int sockfd);                //ready to read
        void Resetcache();

    public:
        Server(std::string port_);
        int Start(int epollfd_);
        void Getepollevents(struct epoll_event ev_) { ev = ev_; }
        void Acceptconnect();
        void Socketwrite(int sockfd, int length);
        void Writefile(int sockfd);
        void Socketread(int sockfd);
        ~Server();
};
int Server::Socketclose(int sockfd) {
    if (close(sockfd) < 0) { 
        err_sys("close:") 
    }
    epoll_ctl(epollfd, EPOLL_CTL_DEL, connect, nullptr);
}
void Server::Record() {}
void Server::Epollwrite(int sockfd) {
    ev.data.fd = sockfd;
    ev.events = EPOLLOUT | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);
}
void Server::Epollread(int sockfd) {
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);
}
void Server::Resetcache() {
    cache.send = 0;
    cache.remaining = 0;
    cache.filefd = 0;
    cache.str = "none";
}


Server::Server(int port_) {
    port = port_;
}
int Server::Start(int epollfd_) {
    epollfd = epollfd_;
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sinaddr.s_addr = htonl(INADDR_ANY);
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    Bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr));
    Listen(listenfd, LISTEN_WAIT);
    std::cout << "Server start!" << endl;
    return listenfd;
}
void Server::Acceptconnect() {
    int sockfd = Accept(listenfd);
    Epollread(sockfd);
}
void Server::Socketwrite(int sockfd, std::string str, int length) {
    cache.str = str;
    cache.remaining = length;
    int n = Write(sockfd, cache);
    if (n < 0) 
        Close(sockfd);
    else if (n > MAX_BUF_SIZE) 
        std::cout << "kernel cache is full." << std::endl;
    else {
        Epollread(sockfd);
        Resetcache();
    }
}
void Server::Writefile(int sockfd, std::string filename) {
    if (Readfile(filename, &cache) < 0) {
        //Httpsend(Not_Found);
        Close(sockfd);
    }
    if (Writefile(sockfd, cache) < 0) {
        Close(sockfd);
    }
    Epollread(sockfd);
    Resetcache();
}
void Server::Socketread(int sockfd, std::string str) {
    int readsize = Read(sockfd, &str);
    if (readsize <= 0)
        Close(sockfd);
    else
        Epollwrite(sockfd);
}




Server::~Server() {
    while(cache) {
		cache = cache.next;
		struct Cache *tmp = cache;
		free(tmp);
	}
}

#endif