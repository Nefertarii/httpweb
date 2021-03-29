#ifndef SERVERPROCESS_H_
#define SERVERPROCESS_H_
#include "servhead.h"

class Server {
    private:
        int listenfd, epollfd;
        std::string readbuf;
        struct Clientinfo clients[MAX_CLI];
        struct epoll_event events[MAX_CLI];
        void TCPlisten(int port);
        void Createclient(int fd);
        void Clientclose(Clientinfo *cli);
        void Setnoblocking(int fd);
        void Record();
        void Epollwrite(int fd, void *cli);       //state ready to write
        void Epollread(int fd, void *cli);         //state ready to read
        void Epolladd(int fd, void *cli);
        void Resetinfo(Clientinfo *cli);

    public:
        Server(int port_);
        void Start();
        void Acceptconnect();
        void Socketwrite(void *cli);
        void Socketread(std::string str, void *cli);
        int Epollfd() { return epollfd; }
        ~Server();
};

void Server::TCPlisten(int port) {
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
    //Clientinfo *client = new Clientinfo;
    for (int i = 0; i <= MAX_CLI; i++) {
        if(i == MAX_CLI) {
        std::cout << "too many client: " << connectfd << std::endl;
        //Clientclose(client);
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
void Server::Epollwrite(int fd, void *cli) {
    struct epoll_event ev;
    ev.events = EPOLLOUT | EPOLLET;
    ev.data.ptr = cli;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}
void Server::Epollread(int fd, void *cli) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = cli;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}
void Server::Epolladd(int fd, void *cli) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = cli;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}
void Server::Resetinfo(Clientinfo *cli) {
    cli->httphead.clear();
    cli->remaining = -1;
    cli->send = -1;
    cli->filefd = -1;
}

Server::Server(int port) {
    for (int i = 0; i != MAX_CLI; i++) {
        clients[i].httphead.clear();
        clients[i].sockfd = -1;
        clients[i].remaining = 0;
        clients[i].send = 0;
        clients[i].filefd = -1;
    }
    epollfd = epoll_create(MAX_CLI);
    TCPlisten(port);
    Epolladd(listenfd, nullptr);
    signal(SIGCHLD, SIG_IGN);
    std::cout << epoll_wait(epollfd, events, MAXEVENTS, TIMEOUT);
    std::cout << "Initialisation complete" << std::endl;
}
void Server::Start() {
    std::cout << "Server start!" << std::endl;
    for (;;) {
        int nfds = epoll_wait(epollfd, events, MAXEVENTS, TIMEOUT);
        if (nfds < 0 && errno != EINTR) {
            err_sys("epoll_wait error:");
            return;
        }
        for (int i = 0; i < nfds; i++) {
            struct epoll_event ev = events[i];
            if(ev.data.fd == listenfd) {
                Acceptconnect();
            } 
            else {
                if (ev.events & EPOLLIN) {
                    std::cout << "read...\t";
                    Socketread(readbuf, ev.data.ptr);
                }
                else if (ev.events & EPOLLOUT) {
                    std::cout << "write...\t";
                    Socketwrite(ev.data.ptr);
                }
            }
        }
    }
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
        Epollread(cli->sockfd, cli);
        Resetinfo(cli);
    }
}
void Server::Socketread(std::string str, void *cli_p) {
    //Clientinfo *cli = static_cast<Clientinfo *>(malloc(sizeof(cli_p)));
    Clientinfo *cli = (Clientinfo *)cli_p;
    int readsize = Read(cli->sockfd, &str);
    if (readsize <= 0) {
        if (readsize < 0) {
            Clientclose(cli);
        }
        cli->httphead = "HTTP/1.1 400 Bad_Request";
        cli->remaining = cli->httphead.length();
        Epollwrite(cli->sockfd, cli);
    }
    else { //读取信息处理 cli cache的大小也在此处设置
        std::string filename;
        Httpprocess(str, &filename);
        std::cout << "Filetype: " << Filetype(filename)
                  << "\tFile:" << filename
                  << "\tReading...";
        if (Readfile(filename, cli)) {
            std::cout << "\tsuccess!" << std::endl;
            Successhead(filename, cli);
            Epollwrite(cli->sockfd, cli);
        }
        else {
            std::cout << "\tfail!" << std::endl;
            cli->httphead = "HTTP/1.1 400 Bad_Request";
            cli->remaining = cli->httphead.length();
            Epollwrite(cli->sockfd, cli);
        }
    }
}
Server::~Server() {
    close(listenfd);
    close(epollfd);
    std::cout << "Server close." << std::endl;
}


#endif