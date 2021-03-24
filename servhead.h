#ifndef SERVHEAD_H_
#define SERVHEAD_H_

#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <ctime>
#include <list>
#include <fstream>
//#include "readfile.h"
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
void Close(int fd);
//void Connect(int fd, const struct sockaddr *sa, socklen_t salen);


int Read(int sockfd, std::string str);
int Readfile(std::string filename, struct Cache cache);
int Write(int socketfd, struct Cache cache);
int Writefile(int sockfd, struct Cache cache);

struct Cache {
    std::string str = "none";
    int remaining = 0;
    int send = 0;
    int filefd = 0;
    struct Cache *next = nullptr;
};
void err_sys(const char *fmt) {
    std::cout << fmt << strerrno(errno);
}




void send_httphead(int connectfd, int filesize, std::string filetype) {
    std::list<std::string> httphead = responehead_html(filesize, filetype);
    for (std::list<std::string>::iterator begin = httphead.begin(); begin != httphead.end(); begin++) {
        Write(connectfd, *begin);
    }
}




int Socket(int family, int type, int protocol) {
    int sockfd = socket(family, type, protocol);
    if (sockfd < 0)
        err_sys("socket error");
    return sockfd;
}
void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
		err_sys("bind error");
}
void Listen(int fd, int backlog) {
	if (listen(fd, backlog) < 0)
		err_sys("listen error");
}
int Accept(int listenfd) {
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen = sizeof(cliaddr);
    while(1) {
        int connectfd = accept(listenfd, (sockaddr *)&cliaddr, &cliaddrlen);
        if (connectfd < 0) {
            if(errno == EINTR)
                continue;
            err_sys("accept error");
            exit(1);
        }
        std::cout << "Get accept form:" << inet_ntoa(cliaddr.sin_addr)
                  << "\tConnectfd: " << connectfd << std::endl;
        return connectfd;
    }
    
}
void Close(int fd) {
	if (close(fd) == -1)
		err_sys("close error");
}
/*void Connect(int fd, const struct sockaddr *sa, socklen_t salen) {
	if (connect(fd, sa, salen) < 0)
		err_sys("connect error");
}*/




int Read(int sockfd, std::string str) {
    char tmp_char[MAX_BUF_SIZE] = {0};
    int readsize = read(sockfd, tmp_char, MAX_BUF_SIZE);
    if (readsize < 0) {
        free(tmp_char);
        if(errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) 
            err_sys("read error:");
        return -1;
    }
    if (readsize == 0) {
        free(tmp_char);
        std::cout << "Client close." << std::endl;
        return 0;
    }
    if (readsize > MAX_BUF_SIZE) {
        free(tmp_char);
        std::cout << "Send to Big." << std::endl;
        return 0;
    }
    str = tmp_char;
    return readsize;
}
int Readfile(std::string filename, struct Cache cache) {
    struct stat filestat_;
    int filestat;
    const char *tmp_char = filename.c_str();
    cache.filefd = open(tmp_char, O_RDONLY);
    if(cache.filefd < 0) {
        std::cout << "Not this file!" << std::endl;
        return -1;
    }
    int filestat = stat(tmp_char, &filestat_);
    if(filestat < 0) {
        err_sys("readfile-stat error:");
        return -1;
    }
    cache.remaining = filestat_.st_size;
    return 0;
}
int Write(int socketfd, struct Cache cache) {
    const char *tmp_char = str.c_str();
    while(cache.remaining) {
        int writesize = write(socketfd, cache.str, MAX_BUF_SIZE);
        if (writesize < 0) {
            if(errno == EINTR)
                continue;           //signal interruption
            else if(errno == EAGAIN || errno == EWOULDBLOCK)
                return MAX_BUF_SIZE + 1;
            else {
                err_sys("write error:");
                return -1;
            }
        }
        if (cache.remaining > MAX_BUF_SIZE) {
            cache.send += MAX_BUF_SIZE;
            cache.remaining -= cache.send;
            cache.str = str.substr(MAX_BUF_SIZE);
        }
        else {
            return 1;
        }
    }
}
int Writefile(int sockfd, struct Cache cache) {  
    while(cache.remaining) {
        int writesize = sendfile(sockfd, cache.filefd, cache.send, MAX_BUF_SIZE);
        if (writesize < 0) {
            if(errno == EINTR)
                continue;
            else
                err_sys("sendfile error:");
        }
        if (cache.remaining > MAX_BUF_SIZE) {
            cache.send += MAX_BUF_SIZE;
            cache.remaining -= cache.send;
        }
        else {
            return 1;
        }
    }
}
#endif
