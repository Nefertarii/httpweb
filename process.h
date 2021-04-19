#ifndef PROCESS_H_
#define PROCESS_H_

#include "jsonprocess.h"
#include "localinfo.h"
#include "record.h"
#include "serverprocess.h"
#include "servhead.h"
#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <ctime>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <random>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

extern const std::string DIR;
extern const std::string PAGE400;
extern const std::string PAGE401;
extern const std::string PAGE403;
extern const std::string PAGE404;

class Process : public Server
{
private:
    CLIENT *cli;

public:
    Process(CLIENT *cli_p) : cli(cli_p) {}
    void Readprocess();
    void GETprocess();
    virtual Method POSTChoess(std::string str_state) = 0;
    std::string Serverstate(int state);
    virtual ~Process();
};

Method Process::Readprocess()
{
    std::cout << "Reading... ";
    int n = HTTPread(cli->get()->sockfd, &(cli->get()->readbuf));
    if (n == 1)
    {
        return Httpprocess(cli->get()->readbuf);
    }
    else if (n == 0)
    {
        std::cout << " size to big.\n";
        Server::BadRequest(400, cli);
        Server::Epollwrite(cli);
        return ERROR;
    }
    else
    {
        std::cout << " read fail.\n";
        Server::Closeclient(cli);
        return ERROR;
    }
}
//GETֻ���ڷ���html/css/js...�ļ�
Method Process::GETprocess(CLIENT *cli)
{
    std::string filedir = DIR; //������ļ��ľ���λ��
    std::string filename;
    std::cout << "Process method:GET...";
    int beg = 5, end = 0;
    while (end <= 100)
    { //��ʼ��ȡҪ����ļ�λ��
        if (cli->get()->readbuf[end + beg] == ' ')
            break;
        end++;
    }
    if (end == 0)
    { //Ĭ�Ϸ���index.html
        filename = "index.html";
    }
    else if (end <= 100)
    { //��ȡ�ļ���
        filename = cli->get()->readbuf.substr(beg, end);
    }
    else if (end > 100)
    { //�ļ���ַҪ�����
        std::cout << " size to long.";
        return ERROR;
    }
    std::cout << " done. "
              << "\nFile: " << filename << " ";
    filedir += filename;
    
    int n = Readfile(filedir, cli);
    if (n == 1)
    { //��ȡ�ɹ�
        std::cout << " Read success." << std::endl;
        Responehead(200, filedir, cli);
        //std::cout << cli->get()->httphead << std::endl;
        return OK;
    }
    else
    { //open/stat����
        std::cout << " Read fali." << std::endl;
        return ERROR;
    }
}
//д��httpͷ֮����������Ϣ �� ǰ��POST�����λ��
Method POSTprocess(std::string readbuf, CLIENT *cli, std::string *info, std::string *location)
{
    //��ȡ����.
    int beg = 0, end = readbuf.length();
    while (beg < 200)
    { //��ȡ���������
        if (readbuf[end - beg] == '\n')
            break;
        beg++;
    }
    if (beg <= 200 && 0 < beg)
    { //��β����ʼ�н�ȡ����
        *info = readbuf.substr((end - beg + 1), beg);
    }
    else
    { //���ݹ�����û������
        *info = "ERROR";
    }

    //��ȡλ�� λ�ò�ͬ����ʽ��ͬ
    beg = 6, end = 0;
    while (end <= 100)
    { //��ȡ�����λ��
        if (readbuf[end + beg] == ' ')
            break;
        end++;
    }
    if (end <= 100 && 0 < end)
    { //��ͷ�н�ȡλ��
        *location = readbuf.substr(beg, end);
    }
    else
    { //Ҫ�������û������
        *location = "ERROR";
    }

    if (*location == "ERROR" || *info == "ERROR")
    {
        return ERROR;
    }
    else
    {
        return OK;
    }
}

class Login : public Process
{
public:
    Login() {}
    virtual Method POSTChoess(std::string str_state);
    ~Login() {}
};
class Resetpassword : public Process
{
};
class Createaccount : public Process
{
};
class Vote : public Process
{
};
class Comment : public Process
{
};
class Content : public Process
{
};
class Readcount : public Process
{
};

//�Ե�¼���ݽ��н�ȡ���ж�
//�ɹ�����1 ���򷵻�-1
int Loginprocess(std::string userinfo, std::string *username, std::string *password)
{
    int beg = 9, end = 0;
    while (1)
    {
        if (userinfo[beg + end] == '&')
        {
            username->assign(userinfo, beg, end);
            break;
        }
        if (end == 51)
        { //���ȡ50λ����/����
            return -1;
        }
        end++;
    }
    beg = beg + end + 10;
    end = 0;
    while (1)
    {
        if (userinfo[beg + end] == '&')
        {
            password->assign(userinfo, beg, end);
            break;
        }
        if (end == 21)
        { //���ȡ20λ����
            return -1;
        }
        end++;
    }
    return 1;
}
//������֤��
#endif