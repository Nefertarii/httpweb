#ifndef PROCESS_H_
#define PROCESS_H_

#include "httphead.h"
#include "servhead.h"

extern const std::string DIR;
extern const std::string PAGE400;
extern const std::string PAGE401;
extern const std::string PAGE403;
extern const std::string PAGE404;

int Loginprocess(std::string userinfo, std::string *username, std::string *password);
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
    int POSTChoess(SERV_PROCESS method);
    int POSTChoess(std::string method);
    std::string Serverstate(int state);
    ~ReadProcess() {}
};
class WriteProcess
{
private:
    CLIENT *cli;

public:
    WriteProcess(CLIENT *cli_p) { cli = cli_p; }
    int Writehead();
    int Writefile();
    int Writeinfo();
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
{                              //GET只用于发送页面文件
    std::string filedir = DIR; //先添加文件的位置
    std::string filename;
    filename = serv::Substr(cli->get()->readbuf, 5, 100, ' '); //GET begin for 5
    if (filename == "0")
    {
        filename = "index.html";
    }
    if (filename == "-1")
    {
        cli->get()->errcode = NOT_THIS_FILE;
        return -1;
    }
    filedir += filename;
    cli->get()->filename = filedir;
    //std::cout << "\n[" << cli->get()->filename << "]\n";
    int n = serv::Readfile(cli);
    if (n == 1)
    { //读取成功
        Responehead(200, filedir, cli);
        cli->get()->status = WRITE_FILE;
        return 1;
    }
    else
    { //open/stat出错
        cli->get()->errcode = FILE_READ_FAIL;
        return -1;
    }
}
int ReadProcess::POSTprocess()
{
    //获取位置 位置不同处理方式不同
    std::string location = serv::Substr(cli->get()->readbuf, 6, 100, ' '); //POST=6
    if (location == "-1")                                                  //to long
    {
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    if (POSTChoess(location) < 0) //post type error
    {
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    //获取数据.
    cli->get()->info = serv::Substr_Revers(cli->get()->readbuf, 200, '\n');
    if (cli->get()->info == "-1")
    { //数据过长或没有设置
        cli->get()->errcode = POST_INFO_ERROR;
        return -1;
    }
    cli->get()->status = Login;
    return 1;
}
int ReadProcess::POSTChoess(std::string method)
{
    if (method == "login")
        cli->get()->status = Login;
    else if (method == "reset")
        cli->get()->status = Reset;
    else if (method == "create")
        cli->get()->status = Create;
    else if (method == "vote_up")
        cli->get()->status = Vote_Up;
    else if (method == "vote_down")
        cli->get()->status = Vote_Down;
    else if (method == "comment")
        cli->get()->status = Comment;
    else if (method == "content")
        cli->get()->status = Content;
    else if (method == "readcount")
        cli->get()->status = Readcount;
    else
    {
        cli->get()->status = SNONE;
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    return 1;
}
int ReadProcess::POSTChoess(SERV_PROCESS method)
{
    std::cout << "\ndone.";
    switch (method)
    {
    case Login:
        cli->get()->Strstate();
        if (POSTLogin())
        {
            cli->get()->info = "{\"Name\":\"gwc\",\"Age\":\"20\",\"session\":\"success\"}";
            cli->get()->remaining = cli->get()->info.length();
            Responehead(200, "login.js", cli);
            cli->get()->status = WRITE_INFO;
            return 1; //成功操作
        }
        cli->get()->info = "{\"session\":\"fail\"}";
        Responehead(200, "login.js", cli);
        cli->get()->errcode = Login_Fail;
        return -1; //失败操作 返回后直接进入写状态
    case Reset:
        /* code */
        //POSTReset();
        break;
    case Create:
        /* code */
        //POSTCreate();
        break;
    case Vote_Up:
        /* code */
        //POSTVote();
        break;
    case Vote_Down:
        /* code */
        //POSTVote();
        break;
    case Comment:
        /* code */
        //POSTComment();
        break;
    case Content:
        /* code */
        //POSTContent();
        break;
    case Readcount:
        /* code */
        //POSTReadcount();
        break;
    default:
        /* code */
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    return -1;
}
int ReadProcess::POSTLogin()
{
    //对数据进行截取、判断
    //成功返回1 否则返回-1
    std::string username = serv::Substr(cli->get()->info, 0, 50, '&');
    std::string password = serv::Substr_Revers(cli->get()->info, 20, '&');
    std::cout << " username:" << username << " password:" << password;
    if (username == "123" && password == "123") //从数据库获取
    {
        return 1;
    }
    /*
    else if (username.length() > MAX_USERNAME || password.length() > MAX_PASSWORD)
    {
        return -1;
    }
    else if (username.length() == 0 || password.length() == 0)
    {
        return -1;
    }
    */
    cli->get()->errcode = Login_Fail;
    cli->get()->status = SNONE;
    cli->get()->Strerror();
    return -1;
}

int WriteProcess::Writehead()
{
    int n = serv::HTTPwrite(cli->get()->httphead, cli->get()->sockfd);
    if (n > 1)
        return 2;
    if (n <= 0)
    {
        if (n == 0)
        {
            cli->get()->errcode = WRITE_AGAIN;
            return 0;
        }
        cli->get()->errcode = WRITE_FAIL;
        return -1;
    }
    cli->get()->httphead.clear();
    return 1;
}
int WriteProcess::Writefile()
{
    if (cli->get()->remaining > WRITE_BUF_SIZE)
    {
        int n = serv::Writefile(cli->get()->send, cli->get()->remaining, cli->get()->sockfd, cli->get()->filefd);
        if (n < 0)
        {
            cli->get()->errcode = WRITE_FAIL;
            return -1;
        }
        cli->get()->send += WRITE_BUF_SIZE;
        cli->get()->writetime += 1;
        cli->get()->remaining -= WRITE_BUF_SIZE;
        return 1;
    }
    else
    {
        int n = serv::Writefile(cli->get()->send, cli->get()->remaining, cli->get()->sockfd, cli->get()->filefd);
        if (n < 0)
        {
            cli->get()->errcode = WRITE_FAIL;
            return -1;
        }
        cli->get()->Reset();
        cli->get()->writetime += 1;
        cli->get()->status = WRITE_OK;
        return 1;
    }
}
int WriteProcess::Writeinfo()
{
    for (;;)
    {
        if (cli->get()->remaining > WRITE_BUF_SIZE)
        {
            int n = serv::HTTPwrite(cli->get()->info, cli->get()->sockfd);
            if (n <= 0)
            {
                if (n == 0)
                {
                    cli->get()->errcode = WRITE_AGAIN;
                    return 0;
                }
                cli->get()->errcode = WRITE_FAIL;
                return -1;
            }
            cli->get()->send += WRITE_BUF_SIZE;
            cli->get()->remaining -= WRITE_BUF_SIZE;
            cli->get()->writetime++;
        }
        else
        {
            int n = serv::HTTPwrite(cli->get()->info, cli->get()->sockfd);
            if (n <= 0)
            {
                if (n == 0)
                {
                    cli->get()->errcode = WRITE_AGAIN;
                    return 0;
                }
                cli->get()->errcode = WRITE_FAIL;
                return -1;
            }
            cli->get()->Reset();
            cli->get()->writetime += 1;
            cli->get()->status = WRITE_OK;
            return 1;
        }
    }
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