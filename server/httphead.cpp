#include "httphead.h"
#include <ctime>

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
std::string Responehead(int state, std::string filename, int bodylength)
{
    std::string httphead;
    httphead += "HTTP/1.1 ";
    httphead += std::to_string(state);
    httphead += Serverstate(state);
    httphead += "Constent_Charset:utf-8\r\n";
    httphead += "Content-Language:zh-CN\r\n";
    httphead += "Connection:Keep-alive\r\n";
    httphead += "Content-Type:";
    httphead += Filetype(filename);
    httphead += "\r\n";
    httphead += "Content-Length:";
    httphead += std::to_string(bodylength);
    httphead += "\r\n";
    //cli->httphead += "cache-control:no-cache\r\n";
    httphead += GMTime();
    httphead += "Server:Gwc/1.0 (C++)\r\n\r\n";

    return httphead;
}