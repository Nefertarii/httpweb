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
{                              //GET只用于发送页面文件
    std::string filedir = DIR; //先添加文件的位置
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
    { //读取成功
        Responehead(200, filedir, cli);
        cli->get()->status = FILE_READ_OK;
        return OK;
    }
    else
    { //open/stat出错
        cli->get()->status = FILE_READ_FAIL;
        return ERROR;
    }
}
Method ReadProcess::POSTprocess()
{
    std::string location;
    //获取数据.
    int beg = 0, end = readbuf.length();
    while (beg < 200)
    { //读取请求的数据
        if (readbuf[end - beg] == '\n')
            break;
        beg++;
    }
    if (beg <= 200 && 0 < beg)
    { //从尾部开始中截取数据
        *info = readbuf.substr((end - beg + 1), beg);
    }
    else
    { //数据过长或没有设置
        *info = "ERROR";
    }

    //获取位置 位置不同处理方式不同
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

//对登录数据进行截取、判断
//成功返回1 否则返回-1
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
        { //最长读取50位名字/邮箱
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
        { //最长读取20位密码
            return -1;
        }
        end++;
    }
    return 1;
}
//返回验证码
//根据状态码不同写入不同的json
void Jsonprocess(int state, CLIENT *cli)
{
    //判断 find name 成功 添加json
    if (state)
    {
        cli->get()->info = "{\"Name\":\"gwc\",\"Age\":\"20\",\"session\":\"success\"}";
    }
    //失败
    else
    {
        cli->get()->info = "{\"session\":\"fail\"}";
    }
    cli->get()->remaining += cli->get()->info.length();
}
#endif