#ifndef HTTPHEAD_H_
#define HTTPHEAD_H_

#include <ctime>
#include <memory>
#include <string>
#include "serverror.h"

#define StatusOK 200           //200 读取成功且合法
#define StatusForbidden 403    //403 禁止访问
#define StatusBadRequest 400   //400 非法请求
#define StatusUnauthorized 401 //401 需要登录 登录失败
#define StatusNotFound 404     //404 访问错误

struct Clientinfo
{
    std::string readbuf;
    std::string httphead;
    std::string info;
    std::string filename;
    SERV_PROCESS status; //next step do what
    SERV_ERR errcode;   //error set
    HTTP_TYPE httptype;
    int writetime;
    int remaining;
    int send;
    int filefd;
    int sockfd;
    bool session;
    Clientinfo()
    {
        readbuf = "none";
        httphead = "none";
        info = "none";
        filename = "none";
        status = PNONE;
        errcode = ENONE;
        httptype = HNONE;
        writetime = 0;
        remaining = 0;
        send = 0;
        filefd = 0;
        sockfd = -1;
        session = false;
    }
    Clientinfo(const Clientinfo &tmp)
    {
        readbuf = tmp.readbuf;
        httphead = tmp.httphead;
        info = tmp.info;
        filename = tmp.filename;
        status = tmp.status;
        errcode = tmp.errcode;
        httptype = tmp.httptype;
        writetime = tmp.writetime;
        remaining = tmp.remaining;
        send = tmp.send;
        filefd = tmp.filefd;
        sockfd = tmp.sockfd;
        session = tmp.session;
    }
    Clientinfo &operator=(struct Clientinfo &&tmp)
    {
        readbuf = tmp.readbuf;
        httphead = tmp.httphead;
        info = tmp.info;
        filename = tmp.filename;
        status = tmp.status;
        errcode = tmp.errcode;
        httptype = tmp.httptype;
        writetime = tmp.writetime;
        remaining = tmp.remaining;
        send = tmp.send;
        filefd = tmp.filefd;
        sockfd = tmp.sockfd;
        session = tmp.session;
        return *this;
    }
    void Reset()
    {
        readbuf.clear();
        httphead.clear();
        info.clear();
        status = PNONE;
        errcode = ENONE;
        httptype = HNONE;
        writetime = 0;
        remaining = 0;
        send = 0;
        filefd = -1;
    }
    const char * Strerror(int codenum)
    {
        return servcode_map[-codenum % -ERRORLAST]; 
    }
    ~Clientinfo() = default;
    //session sockfd在关闭时处理
    //其余的在每次写完成后处理
    //添加/删除数据记得修改Resetinfo()
};
typedef std::shared_ptr<Clientinfo> CLIENT;

//判断请求类型
HTTP_TYPE Httptype(std::string httphead)
{
    if (httphead.find_first_of("GET") == 0)
    {
        return GET;
    }
    else if (httphead.find_first_of("POST") == 0)
    {
        return POST;
    }
    else
    {
        return ERROR;
    }
}

std::string GMTime()
{
    time_t now = time(0);
    tm *ltm = gmtime(&now);
    int GMTDay = ltm->tm_mday;
    int GMTYear = 1900 + ltm->tm_year;
    std::string GMTWeek, GMTMonth, GMTime, GMT;
    switch (ltm->tm_wday)
    {
    case 0:
        GMTWeek = "Sun";
        break;
    case 1:
        GMTWeek = "Mon";
        break;
    case 2:
        GMTWeek = "Tue";
        break;
    case 3:
        GMTWeek = "Wed";
        break;
    case 4:
        GMTWeek = "Thu";
        break;
    case 5:
        GMTWeek = "Fri";
        break;
    case 6:
        GMTWeek = "Sat";
        break;
    }
    switch (ltm->tm_mon)
    {
    case 0:
        GMTMonth = "Jan";
        break;
    case 1:
        GMTMonth = "Feb";
        break;
    case 2:
        GMTMonth = "Mar";
        break;
    case 3:
        GMTMonth = "Apr";
        break;
    case 4:
        GMTMonth = "May";
        break;
    case 5:
        GMTMonth = "Jun";
        break;
    case 6:
        GMTMonth = "Jul";
        break;
    case 7:
        GMTMonth = "Aug";
        break;
    case 8:
        GMTMonth = "Sept";
        break;
    case 9:
        GMTMonth = "Oct";
        break;
    case 10:
        GMTMonth = "Nov";
        break;
    case 11:
        GMTMonth = "Dec";
        break;
    }
    GMTime = std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string(ltm->tm_sec);
    GMT = "Data:" + GMTWeek + ", " + std::to_string(GMTDay) + " " + GMTMonth + " " + std::to_string(GMTYear) + " " + GMTime + " GMT\r\n";
    return GMT;
}
std::string Filetype(std::string filename)
{
    int index = filename.length();
    while (index > 0)
    {
        if (filename[index] == '.' || index == 0)
            break;
        index--;
    }
    std::string suffix(filename, index, filename.length());
    if (suffix == ".html")
        return "text/html";
    else if (suffix == ".data")
        return "application/json";
    else if (suffix == ".css")
        return "text/css";
    else if (suffix == ".js")
        return "text/javascript";
    else if (suffix == ".png")
        return "image/png";
    else if (suffix == ".svg")
        return "image/svg+xml";
    else if (suffix == ".ico")
        return "image/x-icon";
    else
        return "text/plain";
}
//返回处理得到的文件名
std::string Serverstate(int state)
{ //用于http头中的状态码选择
    switch (state)
    {
    case StatusOK: //200
        return " OK\r\n";
    case StatusBadRequest: //400
        return " Bad Request\r\n";
    case StatusUnauthorized: //401
        return " Unauthorized\r\n";
    case StatusForbidden: //403
        return " Forbidden\r\n";
    case StatusNotFound: //404
        return " Not Found\r\n";
    default:
        return " \r\n";
    }
}
void Responehead(int state, std::string filename, CLIENT *cli)
{

    cli->get()->httphead += "HTTP/1.1 ";
    cli->get()->httphead += std::to_string(state);
    cli->get()->httphead += Serverstate(state);
    cli->get()->httphead += "Constent_Charset:utf-8\r\n";
    cli->get()->httphead += "Content-Language:zh-CN\r\n";
    cli->get()->httphead += "Connection:Keep-alive\r\n";
    cli->get()->httphead += "Content-Type:";
    cli->get()->httphead += Filetype(filename);
    cli->get()->httphead += "\r\n";
    cli->get()->httphead += "Content-Length:";
    cli->get()->httphead += std::to_string(cli->get()->remaining);
    cli->get()->httphead += "\r\n";
    //cli->httphead += "cache-control:no-cache\r\n";
    cli->get()->httphead += GMTime();
    cli->get()->httphead += "Server:Gwc/1.0 (C++)\r\n\r\n";

    cli->get()->remaining += cli->get()->httphead.length();
}
#endif
