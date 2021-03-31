#ifndef SERVERPROCESS_H_
#define SERVERPROCESS_H_
#include "servhead.h"

class Server {
    private:
        int listenfd, epollfd;
        std::string readbuf;
        struct Clientinfo clients[MAX_CLI];
        struct epoll_event ev, events[MAX_CLI];
        void Record();
        void TCPlisten(int port);
        void Createclient(int fd);
        void Closeclient(Clientinfo *cli);
        void Epollwrite(Clientinfo *cli);//state ready to write
        void Epollread(Clientinfo *cli);//state ready to read
        void Epolladd(int fd, Clientinfo *cli);
        void Resetinfo(Clientinfo *cli);

    public:
        Server(int port_);
        void Start();
        void Acceptconnect();
        void Socketwrite(void *cli);
        void Socketread(void *cli);
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
    Setreuseaddr(listenfd);
    Bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr));
    Listen(listenfd, LISTEN_WAIT);
}
void Server::Createclient(int connectfd) {
    //Clientinfo *client = new Clientinfo;
    for (int i = 0; i <= MAX_CLI; i++) {
        if(i == MAX_CLI) {
        std::cout << "too many client: " << connectfd << std::endl;
        //Closeclient(client);
        }
        if (clients[i].sockfd < 0) {
            clients[i].sockfd = connectfd;
            //Setnoblocking(connectfd);
            Setbuffer(connectfd);
            Epolladd(connectfd, &clients[i]);
            break;
        }
    }
    
}
void Server::Closeclient(Clientinfo *cli) {
    Close(cli->sockfd);
    epoll_ctl(epollfd, EPOLL_CTL_DEL, cli->sockfd, nullptr);
    cli->sockfd = -1;
    Resetinfo(cli);
    std::cout << "\tClose client " << std::endl;
}
void Server::Record() {}
void Server::Epollwrite(Clientinfo *cli) {
    struct epoll_event ev;
    ev.events = EPOLLOUT | EPOLLET;
    ev.data.ptr = cli;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, cli->sockfd, &ev);
}
void Server::Epollread(Clientinfo *cli) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = cli;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, cli->sockfd, &ev);
}
void Server::Epolladd(int fd, Clientinfo *cli) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = cli;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}
void Server::Resetinfo(Clientinfo *cli) {
    cli->httphead.clear();
    cli->remaining = 0;
    cli->send = 0;
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
    //signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    /*sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    sigprocmask(SIG_BLOCK, &set, NULL);*/
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
            ev = events[i];
            if(ev.data.ptr == nullptr) {
                Acceptconnect();
            } 
            else {
                if (ev.events & EPOLLIN) {
                    Socketread(ev.data.ptr);
                }
                else if (ev.events & EPOLLOUT) {
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
    //head
    std::cout << "Write head";
    int n = Writehead(cli->httphead, cli->sockfd);
    if(n == -1) {
        Closeclient(cli);
        Resetinfo(cli);
    }
    else if (n == 0) {
        std::cout << "kernel cache is full. do again" << std::endl;
    }
    else {
        cli->remaining -= cli->httphead.length();
        cli->httphead.clear();
        std::cout << "   done.";
    }
    //file
    if (cli->remaining > 0) {
        std::cout << " Write file";
        while(1) {
            int n = Writefile(cli->send, cli->remaining, cli->sockfd, cli->filefd);
            if (n == 0) {
                std::cout << "kernel cache is full/Write not over do again" << std::endl;
            }
            else if (n == -1) {
                Closeclient(cli);
                cli->sockfd = -1;
                Resetinfo(cli);
            }
            else if(n == 2){
                cli->send += MAX_BUF_SIZE;
                cli->remaining -= cli->send;
                continue;
            }
            else {
                Resetinfo(cli);
                Epollread(cli);
                std::cout << "   done." << std::endl;
                break;
            }
        }
    }
}
void Server::Socketread(void *cli_p) {
    //Clientinfo *cli = static_cast<Clientinfo *>(malloc(sizeof(cli_p)));
    Clientinfo *cli = (Clientinfo *)cli_p;
    int readsize = Read(cli->sockfd, &readbuf);
    if (readsize <= 0) {
        if (readsize < 0) {
            Closeclient(cli);
        }
        else {
            cli->httphead = "HTTP/1.1 400 Bad_Request";
            cli->remaining = cli->httphead.length();
            Epollwrite(cli);
        }
    }
    else { //读取信息处理 cli cache的大小也在此处设置
        std::string filename;
        Httpprocess(readbuf, &filename);
        std::cout << "  Filetype: " << Filetype(filename)
                  << "  File:" << filename
                  << " Reading...";
        if (Readfile(filename, cli)) {
            std::cout << "  success!" << std::endl;
            Successhead(filename, cli);
        }
        else {
            std::cout << "  fail!" << std::endl;
            cli->httphead = "HTTP/1.1 400 Bad_Request";
            cli->remaining = cli->httphead.length();
        }
        Epollwrite(cli);
    }
}
Server::~Server() {
    close(listenfd);
    close(epollfd);
    std::cout << "Server close." << std::endl;
}


#endif

/*
        std::cout << "cli.httphead:" << cli->httphead << std::endl;
        std::cout << "cli.filefd:" << cli->filefd << std::endl;
        std::cout << "cli.remaining:" << cli->remaining << std::endl;
        std::cout << "cli.send:" << cli->send << std::endl;
        std::cout << "cli.sockfd:" << cli->sockfd << std::endl;
*/