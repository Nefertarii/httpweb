#ifndef SERVHEAD_H_
#define SERVHEAD_H_

#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <cstring>
#include <ctime>
#include <list>
#include <fstream>
#include "httphead.h"

#define OPEN_MAX 64
#define MAX_NAME 64
#define MAX_BUF_SIZE 4000
#define MAX_LINE 20
#define SERV_PORT 80
#define MAXEVENTS 20
#define TIMEOUT 0
#define MAX_CLI 50 //maximum number of client connections
#define LISTEN_WAIT 20
#define INFTIM (-1)
#define DIR "/home/ftp_dir/Webserver/server/"

int Socket(int family, int type, int protocol);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Listen(int fd, int backlog);
int Accept(int listenfd);
//void Connect(int fd, const struct sockaddr *sa, socklen_t salen);


int Read(int sockfd, std::string str);
int Readfile(std::string filename, struct Cache cache);
int Write(int socketfd, struct Cache cache);
int Writefile(int sockfd, struct Cache cache);
void err_sys(const char *fmt) {
    std::cout << fmt << strerror(errno) << std::endl;
}


int Socket(int family, int type, int protocol) {
    int sockfd = socket(family, type, protocol);
    if (sockfd < 0)
        err_sys("socket error:");
    return sockfd;
}
void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
		err_sys("bind error:");
}
void Listen(int fd, int backlog) {
	if (listen(fd, backlog) < 0)
		err_sys("listen error:");
}
int Accept(int listenfd) {
    struct sockaddr_storage  cliaddr;
    socklen_t cliaddrlen = sizeof(cliaddr);
    while(1) {
        int connectfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddrlen);
        if (connectfd < 0) {
            if(errno == EINTR)
                continue;
            err_sys("accept error:");
            exit(1);
        }
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        if (getnameinfo((struct sockaddr *)&cliaddr, cliaddrlen, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
            std::cout << "Get accept form:" << host << " " << service
                      << "\tConnectfd: " << connectfd << std::endl;
        return connectfd;
    }
    
}



int Read(int fd, std::string *str) {
    char tmp_char[MAX_BUF_SIZE] = {0};
    int readsize = read(fd, tmp_char, MAX_BUF_SIZE);
    if (readsize < 0) {
        if(errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
            err_sys("read error:");
            return -1;
        }    
    }
    if (readsize == 0) {
        std::cout << "Client close." << std::endl;
        return -1;
    }
    if (readsize > MAX_BUF_SIZE) {
        std::cout << "Readbuf to Big." << std::endl;
        return 0;
    }
    *str = tmp_char;
    return readsize;
}
int Readfile(std::string filename, struct Clientinfo *cli) {
    struct stat filestat_;
    int filestat;
    const char *tmp_char = filename.c_str();
    cli->write.filefd = open(tmp_char, O_RDONLY);
    if(cli->write.filefd < 0) {
        std::cout << "Not this file!" << std::endl;
        return -1;
    }
    filestat = stat(tmp_char, &filestat_);
    if(filestat < 0) {
        err_sys("readfile-stat error:");
        return -1;
    }
    cli->write.remaining = filestat_.st_size;
    return 0;
}
int Writehttphead(struct Clientinfo *cli) {   //httphead
    const char *tmp_char = cli->write.httphead.c_str();
    int writesize = write(cli->sockfd, tmp_char, MAX_BUF_SIZE);
    if (writesize < 0) {
        if(errno == EINTR)
            return 1;           //signal interruption
        else if(errno == EAGAIN || errno == EWOULDBLOCK) {
            return MAX_BUF_SIZE + 1;
        }
        else {
            err_sys("write error:");
            return -1;
        }
    }
    cli->write.remaining -= cli->write.httphead.length();
    return 0;
}
int Writefile(struct Clientinfo *cli) {  
    while(cli->write.remaining) {
        off_t send = cli->write.send;
        int writesize = sendfile(cli->sockfd, cli->write.filefd, &send, MAX_BUF_SIZE);
        if (writesize < 0) {
            if(errno == EINTR)
                continue;
            else {
                err_sys("sendfile error:");
                return -1;
            }
        }
        if (cli->write.remaining > MAX_BUF_SIZE) {
            cli->write.send += MAX_BUF_SIZE;
            cli->write.remaining -= cli->write.send;
        }
        else {
            cli->write.remaining = 0;
        }
    }
    return 0;
}
#endif
