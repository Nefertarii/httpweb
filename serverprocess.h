#ifndef SERVERPROCESSED_H_
#define SERVERPROCESSED_H_
#include "httphead.h"

class Server {//only epoll
    private:
        int listenfd, epollfd;
        std::vector<std::shared_ptr<Clientinfo>> clients;
        struct epoll_event ev, events[MAX_CLI];
        void Record();
        void Acceptconnect();
        void TCPlisten(int port);
        void Createclient(int fd);
        void Closeclient(CLIENT *cli);
        void Epollwrite(CLIENT *cli);//ready to write
        void Epollread(CLIENT *cli);//ready to read
        void Epolladd(int fd, CLIENT *cli);
        void BadRequest(int statecode, CLIENT *cli);
        //void Sendfile(Clientinfo *cli);

    public:
        Server(int port_);
        void Start();
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
    for (int i = 0; i <= MAX_CLI; i++) {
        if(i == MAX_CLI) {
        std::cout << "too many client: " << connectfd << std::endl;
        }
        if (clients[i]->sockfd < 0) {
            clients[i]->sockfd = connectfd;
            //Setnoblocking(connectfd);
            Setbuffer(connectfd);
            Epolladd(connectfd, &clients[i]);
            break;
        }
    }
    
}
void Server::Closeclient(CLIENT *cli) {
    shutdown(cli->get()->sockfd, SHUT_WR);
    std::cout << "Close client.\n";
    Close(cli->get()->sockfd);
    epoll_ctl(epollfd, EPOLL_CTL_DEL, cli->get()->sockfd, nullptr);
    cli->get()->sockfd = -1;
    cli->get()->session = false;
    cli->get()->Resetinfo();
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
void Server::Epolladd(int fd, CLIENT *cli) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = cli;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}
void Server::BadRequest(int pagecode, Clientinfo *cli) {
    switch (pagecode) {
        case StatusBadRequest: { //400
            Responehead(200, "Page400.html", cli);
            Readfile(PAGE400, cli);
            Epollwrite(cli);
            break;
        }
        case StatusUnauthorized: { //401
            Responehead(200, "Page401.html", cli);
            Readfile(PAGE401, cli);
            Epollwrite(cli);
            break;
        }
        case StatusForbidden: { //403
            Responehead(200, "Page403.html", cli);
            Readfile(PAGE403, cli);
            Epollwrite(cli);
            break;
        }
        case StatusNotFound: { //404
            Responehead(200, "Page404.html", cli);
            Readfile(PAGE404, cli);
            Epollwrite(cli);
            break;
        }
        default: { //404
            Responehead(200, "Page404.html", cli);
            Readfile(PAGE404, cli);
            Epollwrite(cli);
            break;
        }
    }
}

Server::Server(int port) {
    clients.resize(MAX_CLI);
    for (int i = 0; i != MAX_CLI; i++) {
        clients[i] = std::make_shared<Clientinfo>();
    }   
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, Stop);
    epollfd = epoll_create(MAX_CLI);
    TCPlisten(port);
    Epolladd(listenfd, nullptr);
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
    std::cout << "Write head... ";
    if() {

    }
    else {
        if (cli->remaining > 0) {  
            Sendhead(cli);         
            if(cli->filefd != -1) {
                std::cout << " Write file... ";
                Sendfile(cli);
            }
            if (cli->bodyjson.length() > 0) {
                std::cout << " Write json... ";
                Sendjson(cli);
            }
        }
        Epollread(cli);
    }
}
void Server::Socketread(void *cli_p) {
    CLIENT *cli = static_cast<std::shared_ptr<Clientinfo> *>(cli_p);
    if(HTTPread(cli)); {//read success
        Method method = Httpprocess(cli->get()->readbuf);
        if(method == GET) {
            Process process;
            process.GET();
        }
        else if (method == POST) {
            std::string info, location;
            if(POST(readbuf, cli, &info, &location)) {  //POST sucess
                POSTprocessed(location,); //switch(type)
            }
            else {                                      //POST fail
                BadRequest(404, cli);
            }
        }
        else {//GET,POST only
            std::cout << "Error method.";
            Closeclient(cli);
        }
    }
}
Server::~Server() {
    for (int i = 0; i != MAX_CLI; i++) {
        shutdown(clients[i]->sockfd, SHUT_RDWR);
        epoll_ctl(epollfd, EPOLL_CTL_DEL, clients[i]->sockfd, nullptr);
        Close(clients[i]->sockfd);
        clients[i] = nullptr;
    }
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