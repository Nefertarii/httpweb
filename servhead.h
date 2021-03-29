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
#define MAX_BUF_SIZE 4000
#define SERV_PORT 80
#define MAXEVENTS 50
#define TIMEOUT 0
#define MAX_CLI 50      //maximum number of client connections
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
    if (sockfd < 0) {
        err_sys("socket error:");
        exit(1);
    }
    return sockfd;
}
void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0) {
		err_sys("bind error:");
        exit(1);
    }
}
void Listen(int fd, int backlog) {
	if (listen(fd, backlog) < 0) {
		err_sys("listen error:");
        exit(1);
    }
}
int Accept(int listenfd) {
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen = sizeof(cliaddr);
    while(1) {
        int connectfd = accept(listenfd, (sockaddr *)&cliaddr, &cliaddrlen);
        if (connectfd < 0) {
            if(errno == EINTR)
                continue;
            err_sys("accept error:");
            exit(1);
        }
        std::cout << "Get accept form:" << inet_ntoa(cliaddr.sin_addr) << std::endl;
        return connectfd;
    }
    
}
//传入待处理的sockfd 将数据写入str中  成功返回读取的字节数  失败视情况返回 0/-1 
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
//传入要处理的文件名字(若在其他目录传入的名字带相对路径) 
//cli中写入读取的本地文件fd并根据文件大小设置发送的大小
//成功返回1 失败返回0/-1
int Readfile(std::string filename, struct Clientinfo *cli) {
    struct stat filestat_;
    int filestat;
    const char *tmp_char = filename.c_str();
    cli->filefd = open(tmp_char, O_RDONLY);
    if(cli->filefd < 0) {
        std::cout << "Not this file!" << std::endl;
        return 0; 
    }
    filestat = stat(tmp_char, &filestat_);
    if(filestat < 0) {
        err_sys("readfile-stat error:");
        return -1;
    }
    cli->remaining = filestat_.st_size;
    return 1;
}
//在cli中读取并传输本次所需数据的http头数据
//成功返回1 失败返回0/-1
int Writehttphead(struct Clientinfo *cli) {
    const char *tmp_char = cli->httphead.c_str();
    int writesize = write(cli->sockfd, tmp_char, MAX_BUF_SIZE);
    if (writesize < 0) {
        if(errno == EINTR)
            return 0;           //signal interruption
        else if(errno == EAGAIN || errno == EWOULDBLOCK) {
            return MAX_BUF_SIZE + 1;
        }
        else {
            err_sys("write error:");
            return -1;
        }
    }
    cli->remaining -= cli->httphead.length();
    return 1;
}
//在cli中读取保存的本地文件fd 利用系统函数sendfile读取并发送该文件
//成功返回1 失败返回0/-1
int Writefile(struct Clientinfo *cli) {  
    while(cli->remaining) {
        off_t send = cli->send;
        int writesize = sendfile(cli->sockfd, cli->filefd, &send, MAX_BUF_SIZE);
        if (writesize < 0) {
            if(errno == EINTR)
                continue;
            else {
                err_sys("sendfile error:");
                return -1;
            }
        }
        if (cli->remaining > MAX_BUF_SIZE) {
            cli->send += MAX_BUF_SIZE;
            cli->remaining -= cli->send;
        }
        else {
            cli->remaining = 0;
        }
    }
    return 1;
}
#endif
