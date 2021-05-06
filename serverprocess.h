#ifndef SERVERPROCESSED_H_
#define SERVERPROCESSED_H_

#include "localinfo.h"
#include "process.h"
#include "record.h"
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
{
private:
    int listenfd, epollfd, wait; //wait == wait to write or write not over
    std::vector<std::shared_ptr<Clientinfo>> clients;
    std::vector<WriteProcess> waitwrites;
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
    //waitwrites.resize(MAX_CLI);
    for (int i = 0; i != MAX_CLI; i++)
    {
        clients[i] = std::make_shared<Clientinfo>();
        clients[i]->Reset();
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
            else if (ev.events & EPOLLIN)
            {
                CLIENT *cli = static_cast<std::shared_ptr<Clientinfo> *>(ev.data.ptr);
                ReadProcess client(cli);
                std::cout << "\nRead... ";
                if (client.Read())
                {
                    std::cout << "OK. ";
                    switch (cli->get()->httptype)
                    {
                    case GET:
                        if (client.GETprocess())
                        {
                            std::cout << "process success.\n"
                                      << "file:" << cli->get()->filename;
                            Epollwrite(cli);
                            break;
                        }
                        //GET process fail
                        cli->get()->Strerror();
                        BadRequest(404, cli);
                        Epollwrite(cli);
                        break;
                    case POST:
                        if (client.POSTprocess())
                        {
                            client.POSTChoess(cli->get()->status);
                            Epollwrite(cli);
                            break;
                        }
                        cli->get()->Strerror();
                        BadRequest(403, cli);
                        Epollwrite(cli);
                        break;
                    default:
                        cli->get()->Strerror();
                        Closeclient(cli);
                        break;
                    }
                }
                else
                {
                    cli->get()->Strerror();
                    Closeclient(cli);
                }
            }
            else if (ev.events & EPOLLOUT)
            {
                CLIENT *cli = static_cast<std::shared_ptr<Clientinfo> *>(ev.data.ptr);
                WriteProcess client(cli);
                std::cout << "\n";
                int head = client.Writehead();
                if (head >= 1)
                {
                    if (head > 1)
                        ;
                    else
                        std::cout << "Write head... done.  ";
                }
                if (head < 1)
                {
                    cli->get()->Strerror();
                    Closeclient(cli);
                }
                switch (cli->get()->status)
                {
                case WRITE_FILE:
                    if (client.Writefile())
                    {
                        if (cli->get()->remaining == 0)
                        {
                            /*std::cout << "done. write times:"
                                      << cli->get()->writetime;*/
                            Epollread(cli);
                        }
                        else if (cli->get()->remaining > 0)
                        {
                            Epollwrite(cli);
                            cli->get()->status = WRITE_FILE;
                        }
                        break;
                    }
                    else
                    {
                        cli->get()->Strerror();
                        serv::err_sys(" errno:");
                        Closeclient(cli);
                        break;
                    }
                case WRITE_INFO:
                    client.Writeinfo();
                    if (cli->get()->errcode == WRITE_INFO_FAIL)
                    {
                        Closeclient(cli);
                        break;
                    }
                    Epollread(cli);
                    break;
                default:
                    std::cout << "error. ";
                    Closeclient(cli);
                    break;
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
    cli->get()->Reset();
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
        break;
    }
    case StatusUnauthorized:
    { //401
        Responehead(200, "Page401.html", cli);
        cli->get()->filename = PAGE401;
        serv::Readfile(cli);
        break;
    }
    case StatusForbidden:
    { //403
        Responehead(200, "Page403.html", cli);
        cli->get()->filename = PAGE403;
        serv::Readfile(cli);
        break;
    }
    case StatusNotFound:
    { //404
        Responehead(200, "Page404.html", cli);
        cli->get()->filename = PAGE404;
        serv::Readfile(cli);
        break;
    }
    default:
    { //404
        Responehead(200, "Page404.html", cli);
        cli->get()->filename = PAGE404;
        serv::Readfile(cli);
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