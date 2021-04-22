#ifndef SERVERPROCESSED_H_
#define SERVERPROCESSED_H_

#include "localinfo.h"
#include "process.h"
#include "record.h"
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
Server::Server()
{
    clients.resize(MAX_CLI);
    for (int i = 0; i != MAX_CLI; i++)
    {
        clients[i] = std::make_shared<Clientinfo>();
        clients[i]->reset();
    }
    signal(SIGPIPE, SIG_IGN);
    epollfd = epoll_create(MAX_CLI);
    TCPlisten(SERV_PORT);
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
                    //Socketread(ev.data.ptr);
                    CLIENT *cli = static_cast<std::shared_ptr<Clientinfo> *>(ev.data.ptr);
                    ReadProcess client(cli);
                    std::cout << "\nRead... ";
                    client.Read();
                    if (cli->get()->status == HTTP_READ_OK)
                    {
                        std::cout << "OK. ";
                        if (cli->get()->httptype == GET)
                        {
                            std::cout << "method:GET processing...";
                            if (client.GETprocess() == ERROR)
                            {
                                cli->get().Error();
                            }
                            else
                            {
                                if (cli->get()->status == ERROR)
                                {
                                    std::cout << "fail. not this file. ";
                                    cli->get().Error();
                                    BadRequest(404, cli);
                                }
                                else
                                {
                                    std::cout << "done. file:" << cli->get()->filename;
                                    Epollwrite(cli);
                                }
                            }
                        }
                        else if (cli->get()->httptype == POST)
                        {
                            std::cout << "method:POST processing...";
                            client.POSTprocess();
                            if (cli->get()->status == TypeERROR)
                            {
                                std::cout << "fail. POST type Error";
                            }
                            Responehead(403, "403.html", cli);
                            Epollwrite(cli);
                            /*
                            client.POSTprocess();
                            switch (cli->get()->status)
                            {
                            case 1:
                            {
                                std::cout << "case:1: ";
                                break;
                            }
                            default:
                            {
                                std::cout << "case:bad: ";
                                break;
                            }
                            }
                            */
                        }
                        else
                        {
                            std::cout << "error method.";
                            Closeclient(cli);
                        }
                    }
                    else
                    {
                        std::cout << "fail. ";
                        Closeclient(cli);
                    }
                }
                else if (ev.events & EPOLLOUT)
                {
                    //Socketwrite(ev.data.ptr);
                    CLIENT *cli = static_cast<std::shared_ptr<Clientinfo> *>(ev.data.ptr);
                    WriteProcess client(cli);
                    std::cout << "\nWrite ";
                    if (cli->get()->filefd > 0) //write file
                    {
                        std::cout << "file. write head...";
                        client.Writehead();
                        if (cli->get()->status == WRITE_HEAD_OK)
                        {
                            std::cout << "done. write file...";
                            client.Writefile();
                            if (cli->get()->status == WRITE_FILE_FAIL)
                            {
                                std::cout << "fail. ";
                                Closeclient(cli);
                            }
                            else
                            {
                                std::cout << "done. write times:"
                                          << cli->get()->writetime;
                                Epollread(cli);
                            }
                        }
                        else
                        {
                            std::cout << "fail. ";
                            Closeclient(cli);
                        }
                    }
                    else if (cli->get()->info.length() > 0) //write info(json ...)
                    {
                        std::cout << "info. ";
                        client.Writehead();
                        if (cli->get()->status == WRITE_HEAD_OK)
                        {
                            client.Writeinfo();
                            if (cli->get()->status == WRITE_INFO_FAIL)
                            {
                                Closeclient(cli);
                            }
                            else
                            {
                                Epollread(cli);
                            }
                        }
                    }
                    else //error
                    {
                        std::cout << "error. ";
                        Closeclient(cli);
                    }
                }
            }
        }
    }
}
void Server::Stop()
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
    //std::cout << "Bad request" << pagecode;
    switch (pagecode)
    {
    case StatusBadRequest:
    { //400
        Responehead(200, "Page400.html", cli);
        cli->get()->filename = PAGE400;
        serv::Readfile(cli);
        Epollwrite(cli);
        break;
    }
    case StatusUnauthorized:
    { //401
        Responehead(200, "Page401.html", cli);
        cli->get()->filename = PAGE401;
        serv::Readfile(cli);
        Epollwrite(cli);
        break;
    }
    case StatusForbidden:
    { //403
        Responehead(200, "Page403.html", cli);
        cli->get()->filename = PAGE403;
        serv::Readfile(cli);
        Epollwrite(cli);
        break;
    }
    case StatusNotFound:
    { //404
        Responehead(200, "Page404.html", cli);
        cli->get()->filename = PAGE404;
        serv::Readfile(cli);
        Epollwrite(cli);
        break;
    }
    default:
    { //404
        Responehead(200, "Page404.html", cli);
        cli->get()->filename = PAGE404;
        serv::Readfile(cli);
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

#endif

/*
        std::cout << "cli.httphead:" << cli->httphead << std::endl;
        std::cout << "cli.filefd:" << cli->filefd << std::endl;
        std::cout << "cli.remaining:" << cli->remaining << std::endl;
        std::cout << "cli.send:" << cli->send << std::endl;
        std::cout << "cli.sockfd:" << cli->sockfd << std::endl;
*/