#ifndef SERVHEAD_H_
#define SERVHEAD_H_

#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <ctime>
#include <list>
#include <fstream>
#include "readfile.h"
#include "httphead.h"

#define OPEN_MAX 64
#define MAX_LINE 20
#define SERV_PORT 80
#define MAX_BUF_SIZE 1024
#define MAXEVENTS 20
#define TIMEOUT 5
#define MAX_CLI 50 //maximum number of client connections
#define LISTEN_WAIT 20
#define INFTIM (-1)

int Socket(int family, int type, int protocol);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Listen(int fd, int backlog);
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
void Connect(int fd, const struct sockaddr *sa, socklen_t salen);
void Close(int fd);

void epoll_write(int epollfd, int fd, void *ud, int enabled);
void Write(int socketfd, std::string str);
void epoll_read(int epollfd, int fd, void *ud);
int Read(int connectfd, std::string &str);

void err_sys(const char *fmt) {
    std::cout << fmt << std::endl;
    exit(1);
}

void send_httphead(int connectfd, int filesize, std::string filetype) {
    std::list<std::string> httphead = responehead_html(filesize, filetype);
    for (std::list<std::string>::iterator begin = httphead.begin(); begin != httphead.end(); begin++) {
        Write(connectfd, *begin);
    }
}

void send_file(std::list<std::string> file, std::string filetype, int connectfd) {
    if(file.size()==0)
        err_sys("file is empty or not exits   send fail!");
    for (std::list<std::string>::iterator begin = file.begin(); begin != file.end(); begin++) {
        Write(connectfd, *begin);
    }
}

int Socket(int family, int type, int protocol) {
	int	n;
	if ((n = socket(family, type, protocol)) < 0)
		err_sys("socket error");
    return n;
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

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr) {
    int n;
    if ((n = accept(fd, sa, salenptr)) < 0) {
        err_sys("accept error");
    }
    return n;
}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen) {
	if (connect(fd, sa, salen) < 0)
		err_sys("connect error");
}

void Close(int fd) {
	if (close(fd) == -1)
		err_sys("close error");
}

void Write(int socketfd, std::string str) {
    const char *tmp_char = str.c_str();
    write(socketfd, tmp_char, strlen(tmp_char));
}

int Read(int connectfd, std::string &str) {
    char tmp_char[MAX_BUF_SIZE] = {0};
    int Readsize=read(connectfd, tmp_char, MAX_BUF_SIZE);
    str = tmp_char;
    return Readsize;
}
#endif
