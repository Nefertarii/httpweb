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
//GET只用于发送html/css/js...文件
Method ReadProcess::GETprocess()
{
    std::string filedir = DIR; //先添加文件的绝对位置
    std::string filename;
    std::cout << "Process method:GET...";
    int beg = 5, end = 0;
    while (end <= 100)
    { //开始读取要求的文件位置
        if (cli->get()->readbuf[end + beg] == ' ')
            break;
        end++;
    }
    if (end == 0)
    { //默认返回index.html
        filename = "index.html";
    }
    else if (end <= 100)
    { //截取文件名
        filename = cli->get()->readbuf.substr(beg, end);
    }
    else if (end > 100)
    { //文件地址要求过长
        std::cout << " size to long.";
        return ERROR;
    }
    std::cout << " done. "
              << "\nFile: " << filename << " ";
    filedir += filename;

    int n = serv::Readfile(filedir, cli);
    if (n == 1)
    { //读取成功
        std::cout << " Read success." << std::endl;
        Responehead(200, filedir, cli);
        //std::cout << cli->get()->httphead << std::endl;
        return OK;
    }
    else
    { //open/stat出错
        std::cout << " Read fali." << std::endl;
        return ERROR;
    }
}
Method ReadProcess::POSTprocess()
{
    /*
    Method POSTprocess(std::string readbuf, CLIENT * cli, std::string * info, std::string * location)
    {
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
        beg = 6, end = 0;
        while (end <= 100)
        { //读取请求的位置
            if (readbuf[end + beg] == ' ')
                break;
            end++;
        }
        if (end <= 100 && 0 < end)
        { //从头中截取位置
            *location = readbuf.substr(beg, end);
        }
        else
        { //要求过长或没有设置
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
//写入http头之后所带的信息 和 前端POST请求的位置
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