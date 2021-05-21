#ifndef SERVHEAD_H_
#define SERVHEAD_H_

#include "httphead.h"
#include "serverror.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

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
    std::string verifi; //Verificode
    std::string cookie;
    int writetime;
    int remaining;
    int bodylength;
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
    //session sockfd�ڹر�ʱ����
    //�������ÿ��д��ɺ���
    //���/ɾ�����ݼǵ��޸�Resetinfo()
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
    int Random(int num);
    int HTTPread(int sockfd, std::string *str);
    int Readfile(CLIENT *cli);
    int HTTPwrite(std::string info, int sockfd);
    int Writefile(off_t offset, int sockfd, int filefd);
    std::string Substr(std::string readbuf, int beg_i, int maxlength, char end_c);
    std::string Substr_Revers(std::string readbuf, int maxlength, char end_c);
    HTTP_TYPE HTTPtype(std::string httphead);                                   //�ж���������
    int Get_all_files(const std::string dir_, std::vector<std::string> &files); //��ʼ��ʱ��ȡ��֤��
    static std::vector<std::string> VERcode;
}

#endif
