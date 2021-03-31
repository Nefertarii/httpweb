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
#define MAX_BUF_SIZE 16*1024
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


int Read(int fd, std::string *str);
int Readfile(std::string filename, struct Clientinfo *cli);
int Writehead(struct Clientinfo *cli);
int Writefile(struct Clientinfo *cli);
void err_sys(const char *fmt) {
    std::cout << fmt << strerror(errno) << std::endl;
}


int Socket(int family, int type, int protocol) {
    int sockfd = socket(family, type, protocol);
    if (sockfd < 0) {
        err_sys("Socket error:");
        exit(1);
    }
    return sockfd;
}
void Bind(int fd, const struct sockaddr *sa, socklen_t salen) {
	if (bind(fd, sa, salen) < 0) {
		err_sys("Bind error:");
        exit(1);
    }
}
void Listen(int fd, int backlog) {
	if (listen(fd, backlog) < 0) {
		err_sys("Listen error:");
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
            err_sys("Accept error:");
            exit(1);
        }
        std::cout << "Get accept form:" << inet_ntoa(cliaddr.sin_addr) << std::endl;
        return connectfd;
    }
    
}
void Close(int fd) {
    if (close(fd) < 0)
        err_sys("Close error:");
}
void Setnoblocking(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag >= 0) {
        fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    }
}
void Setreuseaddr(int fd) {
    bool Reuseaddr = true;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&Reuseaddr, sizeof(bool)) < 0) {
        err_sys("Setruseaddr error:");
    }
}
void Setbuffer(int fd) {
    int RecvBuf=32*1024;//设置为32K
    int SendBuf=32*1024;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char *)&RecvBuf, sizeof(int)) < 0) {
        err_sys("Setbuffer error:");
    }
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char *)&SendBuf, sizeof(int)) < 0) {
        err_sys("Setbuffer error:");
    }
}


//传入待处理的sockfd 将数据写入str中  
//成功返回读取的字节数  失败返回0/-1 
int Read(int fd, std::string *str) { 
    char tmp_char[MAX_BUF_SIZE] = {0};
    int readsize = read(fd, tmp_char, MAX_BUF_SIZE);
    if (readsize < 0) {
        if(errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
            err_sys("Read error:");
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
        err_sys("Readfile-stat error:");
        return -1;
    }
    cli->remaining = filestat_.st_size;
    return 1;
}
//在cli中读取并传输本次所需数据的http头数据
//成功返回1 失败返回0/-1  0=写未完成 需要再次执行   -1=出错 需关闭连接
int Writehead(std::string httphead,int sockfd) {
    const char *tmp_char = httphead.c_str();
    int num = 0;//记录信号阻塞次数 防止卡住
    while(1) {
        if(num > 10)
            return 0;
        if (write(sockfd, tmp_char, strlen(tmp_char)) < 0) {
            if(errno == EINTR) {
                std::cout << "signal interruption." << std::endl;
                num++;
                continue;//signal interruption
            }
            else if(errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0;//kernel cache full
            }
            else {
                err_sys("Write error:");
                return -1;//another error
            }
        }      
        return 1;
    }
}
//在cli中读取保存的本地文件fd 利用系统函数sendfile读取并发送该文件
//成功返回1/2 失败返回0/-1   
int Writefile(off_t offset, int remaining, int sockfd, int filefd) {
    int num = 0;
    if (remaining > MAX_BUF_SIZE) {
        while (1) {
            if(num > 10)
                return 0;
            int n = sendfile(sockfd, filefd, &offset, MAX_BUF_SIZE);
            if (n < 0) {
                if (errno == EINTR) {
                    std::cout << "signal interruption." << std::endl;
                    num++;
                    continue;
                }
                else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    return 0;
                }
                else {
                    err_sys("Write error:");
                    return -1;
                }
            }
            return 2;
        }
    }
    else {
        while (1) {
            if(num > 10)
                return 0;
            int n = sendfile(sockfd, filefd, &offset, MAX_BUF_SIZE);
            if (n < 0) {
                if(errno == EINTR) {
                    continue;
                }
                else if(errno == EAGAIN || errno == EWOULDBLOCK) {
                    return 0;
                }
                else {
                    err_sys("Write error:");
                    return -1;
                }
            }
            return 1;
        }
    }
}

#endif
