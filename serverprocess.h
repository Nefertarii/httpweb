#ifndef SERVERPROCESSED_H_
#define SERVERPROCESSED_H_
#include "process.h"

extern const std::string DIR;
extern const std::string PAGE400;
extern const std::string PAGE401;
extern const std::string PAGE403;
extern const std::string PAGE404;

class Server
{ //only epoll
private:
    int listenfd, epollfd;
    std::vector<std::shared_ptr<Clientinfo>> clients;
    struct epoll_event ev, events[MAX_CLI];
    //void Sendfile(Clientinfo *cli);
public:
    Server(int port_);
    void Start();
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
    int Epollfd() { return epollfd; }
    ~Server();
};

Server::Server(int port)
{
    clients.resize(MAX_CLI);
    for (int i = 0; i != MAX_CLI; i++)
    {
        clients[i] = std::make_shared<Clientinfo>();
    }
    signal(SIGPIPE, SIG_IGN);
    epollfd = epoll_create(MAX_CLI);
    TCPlisten(port);
    Epolladd(listenfd, nullptr);
    std::cout << "Initialisation complete!\n";
}
void Server::Start()
{
    std::cout << "Server start!\n";
    for (;;)
    {
        int nfds = epoll_wait(epollfd, events, MAXEVENTS, TIMEOUT);
        if (nfds < 0 && errno != EINTR)
        {
            err_sys("epoll_wait error:");
            return;
        }
        for (int i = 0; i < nfds; i++)
        {
            ev = events[i];
            if (ev.data.ptr == nullptr)
            {
                Acceptconnect();
            }
            else
            {
                if (ev.events & EPOLLIN)
                {
                    Socketread(ev.data.ptr);
                }
                else if (ev.events & EPOLLOUT)
                {
                    Socketwrite(ev.data.ptr);
                }
            }
        }
    }
}
void Server::TCPlisten(int port)
{
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
void Server::Createclient(int connectfd)
{
    for (int i = 0; i <= MAX_CLI; i++)
    {
        if (i == MAX_CLI)
        {
            std::cout << "too many client: " << connectfd << std::endl;
        }
        if (clients[i]->sockfd < 0)
        {
            clients[i]->sockfd = connectfd;
            //Setnoblocking(connectfd);
            Setbuffer(connectfd);
            Epolladd(connectfd, &clients[i]);
            break;
        }
    }
}
void Server::Closeclient(CLIENT *cli)
{
    shutdown(cli->get()->sockfd, SHUT_WR);
    std::cout << "Close client.\n";
    Close(cli->get()->sockfd);
    epoll_ctl(epollfd, EPOLL_CTL_DEL, cli->get()->sockfd, nullptr);
    cli->get()->sockfd = -1;
    cli->get()->session = false;
    cli->get()->reset();
}
void Server::Record() {}
void Server::Epollwrite(CLIENT *cli)
{
    struct epoll_event ev;
    ev.events = EPOLLOUT | EPOLLET;
    ev.data.ptr = cli;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, cli->get()->sockfd, &ev);
}
void Server::Epollread(CLIENT *cli)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = cli;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, cli->get()->sockfd, &ev);
}
void Server::Epolladd(int fd, CLIENT *cli)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = cli;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}
void Server::BadRequest(int pagecode, CLIENT *cli)
{
    std::cout << "Bad request" << pagecode;
    switch (pagecode)
    {
    case StatusBadRequest:
    { //400
        Responehead(200, "Page400.html", cli);
        Readfile(PAGE400, cli);
        Epollwrite(cli);
        break;
    }
    case StatusUnauthorized:
    { //401
        Responehead(200, "Page401.html", cli);
        Readfile(PAGE401, cli);
        Epollwrite(cli);
        break;
    }
    case StatusForbidden:
    { //403
        Responehead(200, "Page403.html", cli);
        Readfile(PAGE403, cli);
        Epollwrite(cli);
        break;
    }
    case StatusNotFound:
    { //404
        Responehead(200, "Page404.html", cli);
        Readfile(PAGE404, cli);
        Epollwrite(cli);
        break;
    }
    default:
    { //404
        Responehead(200, "Page404.html", cli);
        Readfile(PAGE404, cli);
        Epollwrite(cli);
        break;
    }
    }
}
void Server::Acceptconnect()
{
    int connectfd = Accept(listenfd);
    Createclient(connectfd);
}
void Server::Socketwrite(void *cli_p)
{
    CLIENT *cli = static_cast<std::shared_ptr<Clientinfo> *>(cli_p);
    std::cout << "Write head... ";

    if (cli->get()->filefd > 0)
    { //设置了filefd只发送文件 否则发送携带的信息info
        std::cout << "Write file... ";
    }

    else
    {
        std::cout << "Write info... ";
    }

    if (cli->get()->httphead.length() != 0)
    {
        int n = HTTPwrite(cli->get()->httphead, cli->get()->sockfd);
        if (n == 1)
        {
            std::cout << "done. ";
            cli->get()->httphead.clear();
        }
        else if (n == 0)
        {
            std::cout << "done. ";
            return;
        }
        else if (n == -1)
        {
            Closeclient(cli);
            return;
        }
    }
    if (cli->get()->filefd > 0)
    { //设置了filefd只发送文件 否则发送携带的信息info
        std::cout << "Write file... ";
        if (cli->get()->remaining > WRITE_BUF_SIZE)
        {
            Writefile(cli->get()->send, cli->get()->remaining, cli->get()->sockfd, cli->get()->filefd);
            cli->get()->send += WRITE_BUF_SIZE;
            cli->get()->remaining -= WRITE_BUF_SIZE;
            cli->get()->t += 1;
        }
        else
        {
            Writefile(cli->get()->send, cli->get()->remaining, cli->get()->sockfd, cli->get()->filefd);
            cli->get()->reset();
            Epollread(cli);
            cli->get()->t += 1;
            std::cout << "done. times:" << cli->get()->t;
        }
    }
    else
    {
        std::cout << "Write info... ";
        if (cli->get()->remaining > WRITE_BUF_SIZE)
        {
            HTTPwrite(cli->get()->info, cli->get()->sockfd);
            cli->get()->send += WRITE_BUF_SIZE;
            cli->get()->remaining -= WRITE_BUF_SIZE;
            cli->get()->t += 1;
        }
        else
        {
            HTTPwrite(cli->get()->info, cli->get()->sockfd);
            cli->get()->reset();
            Epollread(cli);
            cli->get()->t += 1;
            std::cout << "done. times:" << cli->get()->t;
        }
    }
}
void Server::Socketread(void *cli_p)
{
    CLIENT *cli = static_cast<std::shared_ptr<Clientinfo> *>(cli_p);
    Process client(cli);
    Method method = client.Readprocess();
    if (method == GET)
    {
        method = client.GETprocess();
        if(method == OK) 
        {
            Epollwrite(cli);
        }
        else 
        {
            BadRequest(404, cli);
        }
    }
    else if (method == POST)
    {
        std::string info, location;
        client.POSTprocess();
        Closeclient(cli);
    }
    else
    { //GET,POST only
        std::cout << " Error method.";
        BadRequest(404, cli);
    }
}
Server::~Server()
{
    std::cout << "Server closeing... ";
    for (int i = 0; i != MAX_CLI; i++)
    {
        if (clients[i]->sockfd > 0)
        {
            shutdown(clients[i]->sockfd, SHUT_RDWR);
            epoll_ctl(epollfd, EPOLL_CTL_DEL, clients[i]->sockfd, nullptr);
            Close(clients[i]->sockfd);
            clients[i] = nullptr;
        }
    }
    Close(listenfd);
    Close(epollfd);
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