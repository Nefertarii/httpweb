#include "process.h"
#include <iostream>

const std::string HTMLDIR = "/home/gwc/Webserver/Blog/";

int ReadProcess::Read()
{
    int n = serv::HTTPread(cli->get()->sockfd, &(cli->get()->readbuf));
    if (n == 1)
    { //read success.
        cli->get()->httptype = Httptype(cli->get()->readbuf);
        return 1;
    }
    else if (n == 0)
    { //read to large.
        cli->get()->status = READ_FAIL;
        cli->get()->errcode = SIZE_TO_LARGE;
        return -1;
    }
    else
    { //client close
        cli->get()->status = READ_FAIL;
        cli->get()->errcode = CLIENT_CLOSE;
        return -1;
    }
}
int ReadProcess::GETprocess()
{                                  //GET只用于发送页面文件
    std::string filedir = HTMLDIR; //先添加文件的位置
    std::string filename;
    filename = serv::Substr(cli->get()->readbuf, 5, 100, ' '); //GET begin for 5
    if (filename == "0")                                       //default
    {
        filename = "index.html";
    }
    if (filename == "-1")
    {
        cli->get()->status = READ_FAIL;
        cli->get()->errcode = NOT_THIS_FILE;
        return -1;
    }
    filedir += filename;
    cli->get()->filename = filedir;
    //std::cout << "\n[" << cli->get()->filename << "]\n";
    int n = serv::Readfile(cli);
    if (n == 1)
    { //read success
        cli->get()->httphead = Responehead(200, filedir, cli->get()->bodylength);
        cli->get()->remaining += cli->get()->httphead.length();
        return 1;
    }
    else
    { //open/stat出错
        cli->get()->status = READ_FAIL;
        cli->get()->errcode = FILE_READ_FAIL;
        return -1;
    }
}
int ReadProcess::POSTprocess()
{
    //获取位置 位置不同处理方式不同
    std::string location_ = serv::Substr(cli->get()->readbuf, 5, 100, ' '); //POST=6
    std::string location = serv::Substr_Revers(location_, 20, '/');
    std::cout << "location:" << location << "\n";
    if (location == "-1" || location == "0") //to long
    {
        cli->get()->status = READ_FAIL;
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    if (POSTChoess(location) < 0) //post type error
    {
        cli->get()->status = READ_FAIL;
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    //获取数据.
    cli->get()->info = serv::Substr_Revers(cli->get()->readbuf, 200, '\n');
    if (cli->get()->info == "-1" || cli->get()->info == "0")
    { //数据过长或没有设置
        cli->get()->status = READ_FAIL;
        cli->get()->errcode = POST_INFO_ERROR;
        return -1;
    }
    return 1;
}
int ReadProcess::POSTChoess(std::string method)
{
    if (method == "login")
        cli->get()->status = Login;
    else if (method == "reset")
        cli->get()->status = Reset;
    else if (method == "register")
        cli->get()->status = Register;
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
    else if (method == "verification")
        cli->get()->status = Verification;
    else
    {
        cli->get()->status = FAIL;
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    return 1;
}
int ReadProcess::POSTChoess(SERV_STATE method)
{
    switch (method)
    {
    case Login:
    {
        std::string username = serv::Substr(cli->get()->info, 0, 50, '&');
        std::string password = serv::Substr_Revers(cli->get()->info, 20, '&');
        std::string image = Mysqloperation::Mysqllogin(username, password);
        if (image != "error")
        {
            std::initializer_list<std::string> json = {"username", username, "image", image, "session", "success"};
            cli->get()->info = JsonSpliced(json);
            cli->get()->remaining += cli->get()->info.length();
            cli->get()->bodylength += cli->get()->info.length();
            cli->get()->httphead = Responehead(200, "login.js", cli->get()->bodylength);
            cli->get()->remaining += cli->get()->httphead.length();
            cli->get()->status = WRITE_INFO;
            return 1; //成功操作
        }
        cli->get()->info = "{\"session\":\"fail\"}";
        cli->get()->remaining += cli->get()->info.length();
        cli->get()->bodylength += cli->get()->info.length();
        cli->get()->httphead = Responehead(200, "login.js", cli->get()->bodylength);
        cli->get()->remaining += cli->get()->httphead.length();
        cli->get()->status = FAIL;
        cli->get()->errcode = Login_Fail;
        return -1; //失败操作 返回后直接进入写状态
    }
    case Reset:
    {
        /* code */
        //POSTReset();
        break;
    }
    case Register:
    {
        std::string username = serv::Substr(cli->get()->info, 0, 50, '&');
        std::string password = serv::Substr_Revers(cli->get()->info, 20, '&');
        int beg = username.length() + 1;
        int length = cli->get()->info.length() - password.length() - beg - 1;
        std::string email = cli->get()->info.substr(beg, length);
        int n = 0;
        //数据库注册
        n = Mysqloperation::Mysqlregister(username, email, password);
        if (n == 1)
        {
            //std::cout << "username:" << username << "email:" << email << "password:" << password << "\n";
            cli->get()->info = "{\"status\":\"success\"}";
            cli->get()->remaining += cli->get()->info.length();
            cli->get()->bodylength += cli->get()->info.length();
            cli->get()->httphead = Responehead(200, "login.js", cli->get()->bodylength);
            cli->get()->remaining += cli->get()->httphead.length();
            cli->get()->status = WRITE_INFO;
            return 1; //成功操作
        }
        cli->get()->info = "{\"status\":\"fail\"}";
        cli->get()->remaining += cli->get()->info.length();
        cli->get()->bodylength += cli->get()->info.length();
        cli->get()->httphead = Responehead(200, "login.js", cli->get()->bodylength);
        cli->get()->remaining += cli->get()->httphead.length();
        cli->get()->status = FAIL;
        cli->get()->errcode = Register_Fail;
        return -1;
    }
    case Vote_Up:
    {
        /* code */
        //POSTVote();
        break;
    }
    case Vote_Down:
    {
        /* code */
        //POSTVote();
        break;
    }
    case Comment:
    {
        /* code */
        //POSTComment();
        break;
    }
    case Content:
    {
        /* code */
        //POSTContent();
        break;
    }
    case Readcount:
    {
        /* code */
        //POSTReadcount();
        break;
    }
    case Verification:
    {
        int num = serv::Random(99);
        std::initializer_list<std::string> json = {"random", cli->get()->verifi};
        cli->get()->info = JsonSpliced(json);
        cli->get()->remaining += cli->get()->info.length();
        cli->get()->bodylength += cli->get()->info.length();
        cli->get()->httphead = Responehead(200, "login.js", cli->get()->bodylength);
        cli->get()->remaining += cli->get()->httphead.length();
        cli->get()->status = WRITE_INFO;
        return 1;
    }
    default:
    {
        /* code */
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    }
    return -1;
}
int WriteProcess::Writehead()
{
    int n = serv::HTTPwrite(cli->get()->httphead, cli->get()->sockfd);
    if (n >= 1)
    {
        if (cli->get()->filefd > 0)
            cli->get()->status = WRITE_FILE;
        else if (cli->get()->info.length() > 0)
            cli->get()->status = WRITE_INFO;
    }
    if (n <= 0)
    {
        if (n == 0)
        {
            cli->get()->status = WRITE_AGAIN;
            cli->get()->errcode = ERNEL_CACHE_FULL;
            return 0;
        }
        cli->get()->status = WRITE_FAIL;
        cli->get()->errcode = WRITE_HEAD_FAIL;
        return -1;
    }
    cli->get()->httphead.clear();
    return 1;
}
int WriteProcess::Writefile()
{
    if (cli->get()->remaining > WRITE_BUF_SIZE)
    {
        int n = serv::Writefile(cli->get()->send, cli->get()->sockfd, cli->get()->filefd);
        if (n <= 0)
        {
            if (n == 0)
            {
                cli->get()->status = WRITE_AGAIN;
                cli->get()->errcode = ERNEL_CACHE_FULL;
                return 0;
            }
            cli->get()->status = WRITE_FAIL;
            return -1;
        }
        cli->get()->send += WRITE_BUF_SIZE;
        cli->get()->writetime++;
        cli->get()->remaining -= WRITE_BUF_SIZE;
        cli->get()->status = WRITE_AGAIN;
        cli->get()->errcode = SIZE_TO_LARGE;
        return 0;
    }
    else
    {
        int n = serv::Writefile(cli->get()->send, cli->get()->sockfd, cli->get()->filefd);
        if (n <= 0)
        {
            if (n == 0)
            {
                cli->get()->status = WRITE_AGAIN;
                cli->get()->errcode = ERNEL_CACHE_FULL;
                return 0;
            }
            cli->get()->status = WRITE_FAIL;
            return -1;
        }
        cli->get()->writetime++;
        cli->get()->status = WRITE_OK;
        return 1;
    }
}
int WriteProcess::Writeinfo()
{
    if (cli->get()->remaining > WRITE_BUF_SIZE)
    {
        int n = serv::HTTPwrite(cli->get()->info, cli->get()->sockfd);
        if (n <= 0)
        {
            if (n == 0)
            {
                cli->get()->status = WRITE_AGAIN;
                cli->get()->errcode = ERNEL_CACHE_FULL;
                return 0;
            }
            cli->get()->status = WRITE_FAIL;
            return -1;
        }
        cli->get()->send += WRITE_BUF_SIZE;
        cli->get()->remaining -= WRITE_BUF_SIZE;
        cli->get()->writetime++;
        return 0;
    }
    else
    {
        int n = serv::HTTPwrite(cli->get()->info, cli->get()->sockfd);
        if (n <= 0)
        {
            if (n == 0)
            {
                cli->get()->status = WRITE_AGAIN;

                return 0;
            }
            cli->get()->status = WRITE_FAIL;
            return -1;
        }
        cli->get()->Reset();
        cli->get()->writetime++;
        cli->get()->status = WRITE_OK;
        return 1;
    }
}
