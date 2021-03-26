#ifndef SERVERPROCESS_H_
#define SERVERPROCESS_H_
#include "servhead.h"

class Server {
    private:
        int port, listenfd, epollfd;
        Clientinfo clients[MAX_CLI];
        void TCPlisten();
        void Createclient(int fd);
        void Clientclose(Clientinfo *cli);
        void Record();
        void Epollwrite(int fd);              //state ready to write
        void Epollread(int fd);                //state ready to read
        void Epolladd(int fd, Clientinfo *cli);
        void Resetcache(Clientinfo *cli);

    public:
        Server();
        void Start(int epollfd);
        void Acceptconnect();
        void Socketwrite(void *cli_p);
        void Socketread(std::string *str, void *cli_p);
        int Epollfd() { return epollfd; }
        ~Server();
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
void Server::Createclient(int connectfd) {
    Clientinfo *client = nullptr;
    for (int i = 0; i <= MAX_CLI; i++) {
        if(i == MAX_CLI) {
        std::cout << "too many client: " << connectfd << std::endl;
        Clientclose(client);
        }
        if (clients[i].sockfd < 0) {
            clients[i].sockfd = connectfd;
            Epolladd(connectfd, &clients[i]);
            break;
        }
    }
    
}
void Server::Clientclose(Clientinfo *cli) {  
    epoll_ctl(epollfd, EPOLL_CTL_DEL, cli->sockfd, nullptr);
    if (close(cli->sockfd) < 0)
        err_sys("close error:");
    cli->sockfd = -1;
    Resetcache(cli);
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
void Server::Epolladd(int fd, Clientinfo *cli) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = cli;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}
void Server::Resetcache(Clientinfo *cli) {
    cli->write.httphead.clear();
    cli->write.remaining = 0;
    cli->write.send = 0;
    cli->write.filefd = -1;
}

Server::Server() {
    port = 0;
    listenfd = 0;
    epollfd = 0;
    for (int i = 0; i != MAX_CLI; i++) {
        clients[i].sockfd = -1;
        clients[i].write.httphead.clear();
        clients[i].write.remaining = 0;
        clients[i].write.send = 0;
        clients[i].write.filefd = -1;
        clients[i].write.next = nullptr;
    }
    std::cout << "Initialisation complete" << std::endl;
}
void Server::Start(int port_) {
    epollfd = epoll_create(MAX_CLI);
    port = port_;
    TCPlisten();
    Epolladd(listenfd, nullptr);
    std::cout << "Server start!" << std::endl;
}
void Server::Acceptconnect() {
    int connectfd = Accept(listenfd);
    Createclient(connectfd);
}
void Server::Socketwrite(void *cli_p) {
    Clientinfo *cli = static_cast<Clientinfo *>(malloc(sizeof(cli_p)));
    int writesize;
    do {
        writesize = Writehttphead(cli);
        if (writesize < 0)
            Clientclose(cli);
        else if (writesize > MAX_BUF_SIZE) 
            std::cout << "kernel cache is full." << std::endl;
        else {
            continue;
        }
    } while (writesize);
    writesize = Writefile(cli);
    if (writesize < 0)
        Clientclose(cli);
    Epollread(cli->sockfd);
    Resetcache(cli);
}
void Server::Socketread(std::string *str, void *cli_p) {
    Clientinfo *cli = static_cast<Clientinfo *>(malloc(sizeof(cli_p)));
    int readsize = Read(cli->sockfd, str);
    if (readsize <= 0) {
        if (readsize < 0) {
            Clientclose(cli);
        }
        cli->write.httphead = "HTTP/1.1 400 Bad_Request";
        cli->write.remaining = cli->write.httphead.length();
        Epollwrite(cli->sockfd);
    }
    else { //读取信息处理 cli cache的大小也在此处设置
        std::string filename;
        Httpprocess(*str, &filename);
        if (Readfile(filename, cli)) {
            Successhead(filename, cli);
        }
        else {
            cli->write.httphead = "HTTP/1.1 400 Bad_Request";
            cli->write.remaining = cli->write.httphead.length();
            Epollwrite(cli->sockfd);
        }
    }
}
Server::~Server() {
    close(listenfd);
    close(epollfd);
    std::cout << "Server close." << std::endl;
}


#endif