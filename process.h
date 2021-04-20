#ifndef PROCESS_H_
#define PROCESS_H_

#include "localinfo.h"
#include "record.h"
#include "serverprocess.h"

extern const std::string DIR;
extern const std::string PAGE400;
extern const std::string PAGE401;
extern const std::string PAGE403;
extern const std::string PAGE404;

Method ReadProcess::HTTPreadprocess()
{
    std::cout << "Reading... ";
    int n = serv::HTTPread(cli->get()->sockfd, &(cli->get()->readbuf));
    if (n == 1)
    {
        return Httpprocess(cli->get()->readbuf);
    }
    else if (n == 0)
    {
        std::cout << " size to big.\n";
        BadRequest(400, cli);
        Epollwrite(cli);
        return ERROR;
    }
    else
    {
        std::cout << " read fail.\n";
        Closeclient(cli);
        return ERROR;
    }
}
//GETֻ���ڷ���html/css/js...�ļ�
Method ReadProcess::GETprocess()
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

    int n = serv::Readfile(filedir, cli);
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
Method ReadProcess::POSTprocess()
{
    /*
    Method POSTprocess(std::string readbuf, CLIENT * cli, std::string * info, std::string * location)
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
    }*/
    return OK;
}

void WriteProcess::Writehead()
{
    int n = serv::HTTPwrite(cli->get()->httphead, cli->get()->sockfd);
    if (n == 1)
    {
        std::cout << "done. ";
        cli->get()->httphead.clear();
    }
    else if (n == 0)
    {
        std::cout << "done. ";
        return;
    }
    else if (n == -1)
    {
        Closeclient(cli);
        return;
    }
}
void WriteProcess::Writefile()
{
    std::cout << "Write file... ";
    if (cli->get()->remaining > WRITE_BUF_SIZE)
    {
        serv::Writefile(cli->get()->send, cli->get()->remaining, cli->get()->sockfd, cli->get()->filefd);
        cli->get()->send += WRITE_BUF_SIZE;
        cli->get()->remaining -= WRITE_BUF_SIZE;
        cli->get()->t += 1;
    }
    else
    {
        serv::Writefile(cli->get()->send, cli->get()->remaining, cli->get()->sockfd, cli->get()->filefd);
        cli->get()->reset();
        Epollread(cli);
        cli->get()->t += 1;
        std::cout << "done. times:" << cli->get()->t;
    }
}
void WriteProcess::Writeinfo()
{
    std::cout << "Write info... ";
    if (cli->get()->remaining > WRITE_BUF_SIZE)
    {
        serv::HTTPwrite(cli->get()->info, cli->get()->sockfd);
        cli->get()->send += WRITE_BUF_SIZE;
        cli->get()->remaining -= WRITE_BUF_SIZE;
        cli->get()->t += 1;
    }
    else
    {
        serv::HTTPwrite(cli->get()->info, cli->get()->sockfd);
        cli->get()->reset();
        Epollread(cli);
        cli->get()->t += 1;
        std::cout << "done. times:" << cli->get()->t;
    }
}
//д��httpͷ֮����������Ϣ �� ǰ��POST�����λ��
/*
class Login : public ReadProcess
{
};
class Resetpassword : public ReadProcess
{
};
class Createaccount : public ReadProcess
{
};
class Vote : public ReadProcess
{
};
class Comment : public ReadProcess
{
};
class Content : public ReadProcess
{
};
class Readcount : public ReadProcess
{
};
*/

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
//����״̬�벻ͬд�벻ͬ��json
void Jsonprocess(int state, CLIENT *cli)
{
    //�ж� find name �ɹ� ���json
    if (state)
    {
        cli->get()->info = "{\"Name\":\"gwc\",\"Age\":\"20\",\"session\":\"success\"}";
    }
    //ʧ��
    else
    {
        cli->get()->info = "{\"session\":\"fail\"}";
    }
    cli->get()->remaining += cli->get()->info.length();
}
#endif