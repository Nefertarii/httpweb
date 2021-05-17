#ifndef SERVHEAD_H_
#define SERVHEAD_H_

#include "httphead.h"
#include "serverror.h"
#include <arpa/inet.h>
#include <iostream>
#include <memory>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>

enum CONST_DEFINE
{
    OPEN_MAX = 64,
    READ_BUF_SIZE = 16 * 1024,
    WRITE_BUF_SIZE = 20 * 1024,
    SERV_PORT = 80,
    MAXEVENTS = 50,
    TIMEOUT = 0,
    MAX_CLI = 50, //maximum number of client connections
    LISTEN_WAIT = 20,
    INFTIM = (-1),
    MAX_USERNAME = 50,
    MAX_PASSWORD = 50
};

struct Clientinfo
{
    std::string readbuf;
    std::string httphead;
    std::string info;
    std::string filename;
    SERV_STATE status; //next step do what
    SERV_ERR errcode;  //error set
    HTTP_TYPE httptype;
    int writetime;
    int remaining;
    int send;
    int filefd;
    int sockfd;
    bool session;
    Clientinfo();
    Clientinfo(const Clientinfo &tmp);
    Clientinfo &operator=(struct Clientinfo &&tmp);
    void Reset();
    const char *Strerror(int codenum);
    const char *Strerror();
    const char *Strstate(int codenum);
    const char *Strstate();
    ~Clientinfo() { ; }
    //session sockfd在关闭时处理
    //其余的在每次写完成后处理
    //添加/删除数据记得修改Resetinfo()
};
typedef std::shared_ptr<Clientinfo> CLIENT;
namespace serv
{
    int Socket(int family, int type, int protocol);
    void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
    void Listen(int fd, int backlog);
    int Accept(int listenfd);
    void Sys_err(const char *fmt);
    void Close(int fd);
    void Setreuseaddr(int fd);
    void Setnoblocking(int fd);
    void Setbuffer(int fd);
    int Ramdom();
    int HTTPread(int sockfd, std::string *str);
    int Readfile(CLIENT *cli);
    int HTTPwrite(std::string info, int sockfd);
    int Writefile(off_t offset, int remaining, int sockfd, int filefd);
    std::string Substr(std::string readbuf, int beg_i, int maxlength, char end_c);
    std::string Substr_Revers(std::string readbuf, int maxlength, char end_c);
    HTTP_TYPE HTTPtype(std::string httphead); //判断请求类型
}

#endif
