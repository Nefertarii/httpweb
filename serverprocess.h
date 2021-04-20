#ifndef SERVERPROCESSED_H_
#define SERVERPROCESSED_H_

#include "httphead.h"
#include "servhead.h"
#include <iostream>
#include <netdb.h>
#include <signal.h>
#include <sys/epoll.h>
#include <vector>

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
    Server();
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
class ReadProcess : public Server
{
private:
    CLIENT *cli;

public:
    ReadProcess(CLIENT *cli_p) { cli = cli_p; }
    Method HTTPreadprocess();
    Method GETprocess();
    Method POSTprocess();
    //Method POSTChoess(std::string str_state) = 0;
    std::string Serverstate(int state);
    ~ReadProcess() {}
};
class WriteProcess : public Server
{
private:
    CLIENT *cli;

public:
    WriteProcess(CLIENT *cli_p) { cli = cli_p; }
    void Writehead();
    void Writefile();
    void Writeinfo();
    ~WriteProcess() {}
};

Server::Server() {}
void Server::Start()
{
    clients.resize(MAX_CLI);
    for (int i = 0; i != MAX_CLI; i++)
    {
        clients[i] = std::make_shared<Clientinfo>();
    }
    signal(SIGPIPE, SIG_IGN);
    epollfd = epoll_create(MAX_CLI);
    TCPlisten(SERV_PORT);
    Epolladd(listenfd, nullptr);
    std::cout << "Initialisation complete!\n";
    std::cout << "Server start!\n";
    for (;;)
    {
        int nfds = epoll_wait(epollfd, events, MAXEVENTS, TIMEOUT);
        if (nfds < 0 && errno != EINTR)
        {
            serv::err_sys("epoll_wait error:");
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
    listenfd = serv::Socket(AF_INET, SOCK_STREAM, 0);
    serv::Setreuseaddr(listenfd);
    serv::Bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr));
    serv::Listen(listenfd, LISTEN_WAIT);
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
            serv::Setbuffer(connectfd);
            Epolladd(connectfd, &clients[i]);
            break;
        }
    }
}
void Server::Closeclient(CLIENT *cli)
{
    shutdown(cli->get()->sockfd, SHUT_WR);
    std::cout << "Close client.\n";
    serv::Close(cli->get()->sockfd);
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
        serv::Readfile(PAGE400, cli);
        Epollwrite(cli);
        break;
    }
    case StatusUnauthorized:
    { //401
        Responehead(200, "Page401.html", cli);
        serv::Readfile(PAGE401, cli);
        Epollwrite(cli);
        break;
    }
    case StatusForbidden:
    { //403
        Responehead(200, "Page403.html", cli);
        serv::Readfile(PAGE403, cli);
        Epollwrite(cli);
        break;
    }
    case StatusNotFound:
    { //404
        Responehead(200, "Page404.html", cli);
        serv::Readfile(PAGE404, cli);
        Epollwrite(cli);
        break;
    }
    default:
    { //404
        Responehead(200, "Page404.html", cli);
        serv::Readfile(PAGE404, cli);
        Epollwrite(cli);
        break;
    }
    }
}
void Server::Acceptconnect()
{
    int connectfd = serv::Accept(listenfd);
    Createclient(connectfd);
}
void Server::Socketwrite(void *cli_p)
{
    CLIENT *cli = static_cast<std::shared_ptr<Clientinfo> *>(cli_p);
    std::cout << "Write... ";
    WriteProcess client(cli);
    client.Writehead();
    if (cli->get()->filefd > 0)
    { //设置了filefd只发送文件 否则发送携带的信息info
        std::cout << "Write file... ";
        client.Writefile();
    }
    else
    {
        std::cout << "Write info... ";
        client.Writeinfo();
    }
}
void Server::Socketread(void *cli_p)
{
    CLIENT *cli = static_cast<std::shared_ptr<Clientinfo> *>(cli_p);
    ReadProcess client(cli);
    Method method = client.HTTPreadprocess();
    if (method == GET)
    {
        method = client.GETprocess();
        if (method == OK)
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
            serv::Close(clients[i]->sockfd);
            clients[i] = nullptr;
        }
    }
    serv::Close(listenfd);
    serv::Close(epollfd);
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