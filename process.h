#ifndef PROCESS_H_
#define PROCESS_H_

#include "httphead.h"
#include "servhead.h"

extern const std::string DIR;
extern const std::string PAGE400;
extern const std::string PAGE401;
extern const std::string PAGE403;
extern const std::string PAGE404;

class ReadProcess
{
private:
    CLIENT *cli;

public:
    ReadProcess(CLIENT *cli_p) { cli = cli_p; }
    Method Read();
    Method GETprocess();
    Method POSTprocess();
    Method POSTChoess(Method method);
    std::string Serverstate(int state);
    ~ReadProcess() {}
};
class WriteProcess
{
private:
    CLIENT *cli;

public:
    WriteProcess(CLIENT *cli_p) { cli = cli_p; }
    Method Writehead();
    Method Writefile();
    Method Writeinfo();
    ~WriteProcess() {}
};

Method ReadProcess::Read()
{
    int n = serv::HTTPread(cli->get()->sockfd, &(cli->get()->readbuf));
    if (n == 1)
    { //read success.
        cli->get()->httptype = Httptype(cli->get()->readbuf);
        cli->get()->status = HTTP_READ_OK;
        return OK;
    }
    else if (n == 0)
    { //read to large.
        std::cout << " size to big.\n";
        cli->get()->status = HTTP_READ_FAIL;
        return ERROR;
    }
    else
    { //client close
        std::cout << " client close.\n";
        cli->get()->status = HTTP_READ_FAIL;
        return ERROR;
    }
}
Method ReadProcess::GETprocess()
{                              //GETֻ���ڷ���ҳ���ļ�
    std::string filedir = DIR; //������ļ���λ��
    std::string filename;
    filename = serv::Substr(cli->get()->readbuf, 5); //GET=5
    if (filename.length() < 1)
    {
        cli->get()->status = NOT_THIS_FILE;
        return ERROR;
    }
    filedir += filename;
    cli->get()->filename = filedir;
    //std::cout << "\n[" << cli->get()->filename << "]\n";
    int n = serv::Readfile(cli);
    if (n == 1)
    { //��ȡ�ɹ�
        Responehead(200, filedir, cli);
        cli->get()->status = FILE_READ_OK;
        return OK;
    }
    else
    { //open/stat����
        cli->get()->status = FILE_READ_FAIL;
        return ERROR;
    }
}
Method ReadProcess::POSTprocess()
{
    std::string location;
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
    serv::Substr(readbuf, 6); //POST=6

    if (*location == "ERROR" || *info == "ERROR")
    {
        return ERROR;
    }
    else
    {
        return OK;
    }
    return OK;
}
Method ReadProcess::POSTChoess(Method method)
{
    switch (method)
    {
    case Login:
        /* code */
        break;
    case Login:
        /* code */
        break;
    case Reset:
        /* code */
        break;
    case Create:
        /* code */
        break;
    case Vote_Up:
        /* code */
        break;
    case Vote_Down:
        /* code */
        break;
    case Comment:
        /* code */
        break;
    case Content:
        /* code */
        break;
    case Readcount:
        /* code */
        break;
    default:
        /* code */
        break;
    }
}
Method WriteProcess::Writehead()
{
    int n = serv::HTTPwrite(cli->get()->httphead, cli->get()->sockfd);
    if (n == 1)
    {
        cli->get()->httphead.clear();
        cli->get()->status = WRITE_HEAD_OK;
        return OK;
    }
    else if (n == 0)
    {
        cli->get()->status = WRITE_HEAD_FAIL;
        return ERROR;
    }
    else if (n == -1)
    {
        cli->get()->status = WRITE_HEAD_FAIL;
        return ERROR;
    }
    return ERROR;
}
Method WriteProcess::Writefile()
{
    while (1)
    {
        if (cli->get()->remaining > WRITE_BUF_SIZE)
        {
            int n = serv::Writefile(cli->get()->send, cli->get()->remaining, cli->get()->sockfd, cli->get()->filefd);
            if (n < 0)
            {
                cli->get()->status = WRITE_FILE_FAIL;
                return ERROR;
            }
            cli->get()->send += WRITE_BUF_SIZE;
            cli->get()->remaining -= WRITE_BUF_SIZE;
            cli->get()->writetime += 1;
        }
        else
        {
            int n = serv::Writefile(cli->get()->send, cli->get()->remaining, cli->get()->sockfd, cli->get()->filefd);
            if (n < 0)
            {
                cli->get()->status = WRITE_FILE_FAIL;
                return ERROR;
            }
            cli->get()->reset();
            cli->get()->writetime += 1;
            cli->get()->status = WRITE_FILE_OK;
            return OK;
        }
    }
}
Method WriteProcess::Writeinfo()
{
    for (;;)
    {
        if (cli->get()->remaining > WRITE_BUF_SIZE)
        {
            int n = serv::HTTPwrite(cli->get()->info, cli->get()->sockfd);
            if (n < 0)
            {
                cli->get()->status = WRITE_INFO_FAIL;
                return ERROR;
            }
            cli->get()->send += WRITE_BUF_SIZE;
            cli->get()->remaining -= WRITE_BUF_SIZE;
            cli->get()->writetime++;
        }
        else
        {
            int n = serv::HTTPwrite(cli->get()->info, cli->get()->sockfd);
            if (n < 0)
            {
                cli->get()->status = WRITE_INFO_FAIL;
                return ERROR;
            }
            cli->get()->reset();
            cli->get()->writetime += 1;
            cli->get()->status = WRITE_INFO_OK;
            return OK;
        }
    }
}

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