#include "servprocess.h"
#include "../record/record.h"

const std::string DATADIR = "/home/ftp_dir/Webserver/Data/";
const std::string PAGE400 = "/home/ftp_dir/Webserver/Blog/Errorpage/Page400.html";
const std::string PAGE401 = "/home/ftp_dir/Webserver/Blog/Errorpage/Page401.html";
const std::string PAGE403 = "/home/ftp_dir/Webserver/Blog/Errorpage/Page403.html";
const std::string PAGE404 = "/home/ftp_dir/Webserver/Blog/Errorpage/Page404.html";

Server::Server()
{
    clients.resize(MAX_CLI);
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
            serv::Sys_err("epoll_wait error:");
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
                if (client.Read())
                {
                    switch (cli->get()->httptype)
                    {
                    case GET:
                        client.GETprocess();
                        if (cli->get()->status == READ_FAIL)
                        {
                            cli->get()->Strerror();
                            BadRequest(404, cli);
                            Epollwrite(cli);
                            break;
                        }
                        std::cout << "Method:GET process success.\n"
                                  << "File:" << cli->get()->filename << "\n";
                        Epollwrite(cli);
                        break;
                    case POST:
                        client.POSTprocess();
                        if (cli->get()->status == READ_FAIL)
                        {
                            cli->get()->Strerror();
                            BadRequest(403, cli);
                            Epollwrite(cli);
                            break;
                        }
                        std::cout << cli->get()->Strstate();
                        client.POSTChoess(cli->get()->status);
                        if(cli->get()->status == FAIL)
                        {
                            std::cout << cli->get()->Strstate() << " ";
                            std::cout << cli->get()->Strerror() << "\n";
                        }
                        else
                        {
                            std::cout << " Success!\n";
                        }
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
                    if (cli->get()->errcode == CLIENT_CLOSE)
                    {
                        std::cout << cli->get()->Strerror() << std::endl;
                        Closeclient(cli);
                    }
                    else
                    {
                        std::cout << cli->get()->Strerror() << std::endl;
                        BadRequest(400, cli);
                    }
                }
            }
            else if (ev.events & EPOLLOUT)
            {
                CLIENT *cli = static_cast<std::shared_ptr<Clientinfo> *>(ev.data.ptr);
                WriteProcess client(cli);
                client.Writehead();
                switch (cli->get()->status)
                {
                case WRITE_FILE:
                    client.Writefile();
                    switch (cli->get()->status)
                    {
                    case WRITE_OK:
                        std::cout << cli->get()->Strstate() << "\n";
                        cli->get()->Reset();
                        Epollread(cli);
                        break;
                    case WRITE_AGAIN:
                        std::cout << cli->get()->Strstate() << " ";
                        std::cout << cli->get()->Strerror() << "\n";
                        Epollwrite(cli);
                        break;
                    case WRITE_FAIL:
                        std::cout << cli->get()->Strstate() << " ";
                        std::cout << cli->get()->Strerror() << "\n";
                        Closeclient(cli);
                        break;
                    default:
                        std::cout << cli->get()->Strstate() << " ";
                        std::cout << cli->get()->Strerror() << "\n";
                        Closeclient(cli);
                        break;
                    }
                    break;
                case WRITE_INFO:
                    client.Writeinfo();
                    switch (cli->get()->status)
                    {
                    case WRITE_OK:
                        std::cout << cli->get()->Strstate() << "\n";
                        cli->get()->Reset();
                        Epollread(cli);
                        break;
                    case WRITE_AGAIN:
                        std::cout << cli->get()->Strstate() << " ";
                        std::cout << cli->get()->Strerror() << "\n";
                        Epollwrite(cli);
                        break;
                    case WRITE_FAIL:
                        std::cout << cli->get()->Strstate() << " ";
                        std::cout << cli->get()->Strerror() << "\n";
                        Closeclient(cli);
                        break;
                    default:
                        std::cout << cli->get()->Strstate() << " ";
                        std::cout << cli->get()->Strerror() << "\n";
                        Closeclient(cli);
                        break;
                    }
                    break;
                case WRITE_AGAIN:
                    std::cout << cli->get()->Strstate() << " ";
                    std::cout << cli->get()->Strerror() << "\n";
                    Epollwrite(cli);
                    break;
                default:
                    std::cout << cli->get()->Strerror();
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
    memset(&servaddr, 0, sizeof(servaddr));
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
    case StatusBadRequest: //400
        Responehead(200, "Page400.html", cli->get()->bodylength);
        cli->get()->remaining += cli->get()->httphead.length();
        cli->get()->filename = PAGE400;
        serv::Readfile(cli);
        break;
    case StatusUnauthorized: //401
        Responehead(200, "Page401.html", cli->get()->bodylength);
        cli->get()->remaining += cli->get()->httphead.length();
        cli->get()->filename = PAGE401;
        serv::Readfile(cli);
        break;
    case StatusForbidden: //403
        Responehead(200, "Page403.html", cli->get()->bodylength);
        cli->get()->remaining += cli->get()->httphead.length();
        cli->get()->filename = PAGE403;
        serv::Readfile(cli);
        break;
    case StatusNotFound: //404
        Responehead(200, "Page404.html", cli->get()->bodylength);
        cli->get()->remaining += cli->get()->httphead.length();
        cli->get()->filename = PAGE404;
        serv::Readfile(cli);
        break;
    default: //404
        Responehead(200, "Page404.html", cli->get()->bodylength);
        cli->get()->remaining += cli->get()->httphead.length();
        cli->get()->filename = PAGE404;
        serv::Readfile(cli);
        break;
    }
}
void Server::Acceptconnect()
{
    int connectfd = serv::Accept(listenfd);
    Createclient(connectfd);
}