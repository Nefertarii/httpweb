#ifndef SERVHEAD_H_
#define SERVHEAD_H_

#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
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
#include <csignal>
#include <ctime>
#include <random>
#include <vector>
#include <memory>
#include "process.h"
#include "localinfo.h"
#include "jsonprocess.h"
#include "record.h"

#define OPEN_MAX 64
#define MAX_BUF_SIZE 8*1024
#define BUFFER_SIZE 2*1024
#define SERV_PORT 80
#define MAXEVENTS 50
#define TIMEOUT 0
#define MAX_CLI 50      //maximum number of client connections
#define LISTEN_WAIT 20
#define INFTIM (-1)
#define CLIENT std::shared_ptr<Clientinfo>
#define DIR  "/home/ftp_dir/Webserver/Blog/"
#define PAGE400 "/home/ftp_dir/Webserver/Blog/Errorpage/Page400.html"
#define PAGE401 "/home/ftp_dir/Webserver/Blog/Errorpage/Page401.html"
#define PAGE403 "/home/ftp_dir/Webserver/Blog/Errorpage/Page403.html"
#define PAGE404 "/home/ftp_dir/Webserver/Blog/Errorpage/Page404.html"



struct Clientinfo {
    std::string readbuf;
    std::string httphead;
    std::string info;
    int remaining;
    int send;
    int filefd;
    int sockfd;
    bool session;
    Clientinfo() : readbuf("none"), httphead("none"), info("none"), remaining(0), send(0), filefd(0), sockfd(0), session(false){}
    Clientinfo(const Clientinfo &tmp) : readbuf(tmp.readbuf), httphead(tmp.httphead), info(tmp.info), remaining(tmp.remaining), send(tmp.send), filefd(tmp.filefd), sockfd(tmp.sockfd), session(tmp.session){}
    Clientinfo &operator=(struct Clientinfo &&tmp_) {
        readbuf = tmp.readbuf;
        httphead = tmp_.httphead;
        info = tmp_.info;
        remaining = tmp_.remaining;
        send = tmp_.send;
        filefd = tmp_.filefd;
        sockfd = tmp_.sockfd;
        session = tmp_.session;
        return *this;
    }
    void Resetinfo() {
        readbuf.clear();
        httphead.clear();
        info.clear();
        remaining = 0;
        send = 0;
        filefd = -1;
    }
    ~Clientinfo() = default;
    //session sockfd在关闭时处理
    //其余的在每次写完成后处理
    //添加/删除数据记得修改Resetinfo()
};








int Socket(int family, int type, int protocol);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Listen(int fd, int backlog);
int Accept(int listenfd);
//void Connect(int fd, const struct sockaddr *sa, socklen_t salen);


//传入待处理的sockfd 将数据写入str中  
//成功返回读取的字节数  失败返回0/-1
int HTTPread(CLIENT *cli) { 
    char tmp_char[MAX_BUF_SIZE] = {0};
    int readsize = read(cli->get()->sockfd, tmp_char, MAX_BUF_SIZE);
    if (readsize < 0) {
        if(errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
            err_sys("Read error:");
            Closeclient(cli);//读出错
        }    
    }
    else if (readsize == 0) {
        std::cout << "Client close.\n";
        Closeclient(cli);//对端关闭 FIN
    }
    else if (readsize > MAX_BUF_SIZE) {
        return 0;
    }
    *str = tmp_char;
    return 1;
}
//传入要处理的文件名字(若在其他目录传入的名字带相对路径) 
//成功返回1 cli中写入读取的本地文件fd并根据文件大小设置剩余发送的大小
//失败返回0/-1
int Readfile(std::string filename, struct Clientinfo *cli) {
    struct stat filestat_;
    int filestat;
    const char *tmp_char = filename.c_str();

    cli->filefd = open(tmp_char, O_RDONLY);
    if(cli->filefd < 0) {
        err_sys("func(Readfile) open error:");
        return 0; 
    }

    filestat = stat(tmp_char, &filestat_);
    if(filestat < 0) {
        err_sys("func(Readfile) stat error:");
        return -1;
    }
    cli->remaining = filestat_.st_size;
    return 1;
}
//在cli中读取并传输本次所需数据的http头数据
//成功返回1 失败返回0/-1  0=写未完成 需要再次执行   -1=出错 需关闭连接
int HTTPwrite(std::string info,int sockfd) {
    const char *tmp_char = info.c_str();
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
void err_sys(const char *fmt) {
    std::cout << fmt << strerror(errno) << std::endl;
}


int Socket(int family, int type, int protocol) {
    int sockfd = socket(family, type, protocol);
    if (sockfd < 0) {
        err_sys("Socket error:");
        exit(errno);
    }
    return sockfd;
}
void Bind(int fd, const struct sockaddr *sa, socklen_t salen) {
	if (bind(fd, sa, salen) < 0) {
		err_sys("Bind error:");
        exit(errno);
    }
}
void Listen(int fd, int backlog) {
	if (listen(fd, backlog) < 0) {
		err_sys("Listen error:");
        exit(errno);
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
            exit(errno);
        }
        std::cout << "Get accept form:" << inet_ntoa(cliaddr.sin_addr) << std::endl;
        return connectfd;
    }
    
}
void Close(int fd) {
    if (close(fd) < 0)
        err_sys("Close error:");
}
void Setnoblocking(int fd) { //不阻塞
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag >= 0) {
        fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    }
}
void Setreuseaddr(int fd) { //重用地址 避免FIN_WAIT状态
    bool Reuseaddr = true;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&Reuseaddr, sizeof(bool)) < 0) {
        err_sys("Setruseaddr error:");
    }
}
void Setbuffer(int fd) { //设置缓存
    int RecvBuf = BUFFER_SIZE; 
    int SendBuf = BUFFER_SIZE;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char *)&RecvBuf, sizeof(int)) < 0) {
        err_sys("Setbuffer error:");
    }
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char *)&SendBuf, sizeof(int)) < 0) {
        err_sys("Setbuffer error:");
    }
}
int Ramdom() {
    std::random_device rd;
    std::mt19937 mt(rd());
    return = mt() % 100;
}



#endif
