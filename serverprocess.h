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
        void Setnoblocking(int fd);
        void Record();
        void Epollwrite(int fd);              //state ready to write
        void Epollread(int fd);                //state ready to read
        void Epolladd(int fd, Clientinfo *cli);
        void Resetinfo(Clientinfo *cli);

    public:
        Server();
        void Start(int epollfd);
        void Acceptconnect();
        void Socketwrite(void *cli);
        void Socketread(std::string *str, void *cli);
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
            Setnoblocking(connectfd);
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
    Resetinfo(cli);
}
void Server::Setnoblocking(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag >= 0) {
        fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    }
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
    std::cout << "Client create." << std::endl;
}
void Server::Resetinfo(Clientinfo *cli) {
    cli->httphead.clear();
    cli->remaining = 0;
    cli->send = 0;
    cli->filefd = -1;
}

Server::Server() {
    port = 0;
    listenfd = 0;
    epollfd = 0;
    for (int i = 0; i != MAX_CLI; i++) {
        clients[i].httphead.clear();
        clients[i].sockfd = -1;
        clients[i].remaining = 0;
        clients[i].send = 0;
        clients[i].filefd = -1;
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
    Clientinfo *cli = (Clientinfo *)cli_p;
    if (cli->httphead.length()>0) {
        Writehttphead(cli);
        Clientclose(cli);
    }
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
    else {
        Epollread(cli->sockfd);
        Resetinfo(cli);
    }
}
void Server::Socketread(std::string *str, void *cli_p) {
    //Clientinfo *cli = static_cast<Clientinfo *>(malloc(sizeof(cli_p)));
    Clientinfo *cli = (Clientinfo *)cli_p;
    int readsize = Read(cli->sockfd, str);
    if (readsize <= 0) {
        if (readsize < 0) {
            Clientclose(cli);
        }
        cli->httphead = "HTTP/1.1 400 Bad_Request";
        cli->remaining = cli->httphead.length();
        Epollwrite(cli->sockfd);
    }
    else { //读取信息处理 cli cache的大小也在此处设置
        std::string filename;
        Httpprocess(str, &filename);
        if (Readfile(filename, cli)) {
            Successhead(filename, cli);
        }
        else {
            cli->httphead = "HTTP/1.1 400 Bad_Request";
            cli->remaining = cli->httphead.length();
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