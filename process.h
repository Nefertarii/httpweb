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
    int Read();
    int GETprocess();
    int POSTprocess();
    int POSTLogin();
    int POSTReset();
    int POSTCreate();
    int POSTVote();
    int POSTComment();
    int POSTContent();
    int POSTReadcount();
    void POSTChoess(std::string method);
    int POSTChoess(SERV_PROCESS method);
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

int ReadProcess::Read()
{
    int n = serv::HTTPread(cli->get()->sockfd, &(cli->get()->readbuf));
    if (n == 1)
    { //read success.
        cli->get()->httptype = Httptype(cli->get()->readbuf);
        cli->get()->status = HTTP_READ_OK;
        return 1;
    }
    else if (n == 0)
    { //read to large.
        std::cout << " size to big.\n";
        cli->get()->errcode = SIZE_TO_LARGE;
        return -1;
    }
    else
    { //client close
        std::cout << " client close.\n";
        cli->get()->errcode = CLIENT_CLOSE;
        return -1;
    }
}
int ReadProcess::GETprocess()
{                              //GETֻ���ڷ���ҳ���ļ�
    std::string filedir = DIR; //������ļ���λ��
    std::string filename;
    filename = serv::Substr(cli->get()->readbuf, 5); //GET begin for 5 
    if (filename.length() < 1)
    {
        cli->get()->errcode = NOT_THIS_FILE;
        return -1;
    }
    filedir += filename;
    cli->get()->filename = filedir;
    //std::cout << "\n[" << cli->get()->filename << "]\n";
    int n = serv::Readfile(cli);
    if (n == 1)
    { //��ȡ�ɹ�
        Responehead(200, filedir, cli);
        cli->get()->status = FILE_READ_OK;
        return 1;
    }
    else
    { //open/stat����
        cli->get()->errcode = FILE_READ_FAIL;
        return -1;
    }
}
int ReadProcess::POSTprocess()
{
    //��ȡλ�� λ�ò�ͬ����ʽ��ͬ
    std::string location = serv::Substr(cli->get()->readbuf, 6); //POST=6
    if(location.length() < 1)
    {
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    if (POSTChoess(location) < 0) {
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    //��ȡ����.
    int beg = 0, end = cli->get()->readbuf.length();
    while (beg < 200)
    { //��ȡ���������
        if (readbuf[end - beg] == '\n')
            break;
        beg++;
    }
    if (beg <= 200 && 0 < beg)
    { //��β����ʼ�н�ȡ�����ݴ���info�� ����ɺ�׼��дʱ�ٸ���
        cli->get()->info = readbuf.substr((end - beg + 1), beg);
    }
    else
    { //���ݹ�����û������
        cli->get()->errcode = POST_INFO_ERROR;
        return -1;
    }
    return 1;




}
int ReadProcess::POSTChoess(std::string method)
{
    if (method == "Login")
        cli->get()->status = Login;
    else if (method == "Reset")
        cli->get()->status = Reset;
    else if (method == "Create")
        cli->get()->status = Create;
    else if (method == "Vote_Up")
        cli->get()->status = Vote_Up;
    else if (method == "Vote_Down")
        cli->get()->status = Vote_Down;
    else if (method == "Comment")
        cli->get()->status = Comment;
    else if (method == "Content")
        cli->get()->status = Content;
    else if (method == "Readcount")
        cli->get()->status = Readcount;
    else {
        cli->get()->status = PNONE;
        return -1;
    }
    return 1;
}
int ReadProcess::POSTChoess(SERV_PROCESS method)
{
    switch (method)
    {
    case Login:
        std::cout << "done. Login...";
        POSTLogin();
        break;
    case Reset:
        /* code */
        POSTReset();
        break;
    case Create:
        /* code */
        POSTCreate();
        break;
    case Vote_Up:
        /* code */
        POSTVote();
        break;
    case Vote_Down:
        /* code */
        POSTVote();
        break;
    case Comment:
        /* code */
        POSTComment();
        break;
    case Content:
        /* code */
        POSTContent();
        break;
    case Readcount:
        /* code */
        PSOTReadcount();
        break;
    default:
        /* code */
        return ERROR;
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
    for (;;)
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
    for (;;)
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
    for (;;)
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