#ifndef HTTPHEAD_H_
#define HTTPHEAD_H_

struct Clientinfo {
    std::string httphead;
    int sockfd;
    int remaining;
    int send;
    int filefd;
    Clientinfo() {
        httphead = "";
        sockfd = -1;
        remaining = -1;
        send = -1;
        filefd = -1;
    }
    Clientinfo operator=(struct Clientinfo tmp_) {
        struct Clientinfo tmp;
        tmp.httphead = tmp_.httphead;
        tmp.sockfd = tmp_.sockfd;
        tmp.remaining = tmp_.remaining;
        tmp.send = tmp_.send;
        tmp.filefd = tmp_.filefd;
        return tmp;
    }
};


enum STATE_CODE
{
    OK = 200,
    No_Content = 204,
    Partial_Content = 206,

    Moved_Permanently = 301,
    Found = 302,
    See_Other = 303,
    Not_Modified = 304,
    Temporaty_Redirect = 307,

    Bad_Request = 400,
    Unauthorized = 401,
    Forbidden = 403,
    Not_Found = 404,

    Unsupported_Media_Type = 415,
    Internal_Server_Error = 500,
    Server_Unavailable = 503
};
std::string GMTime() {
    time_t now = time(0);
    tm *ltm = gmtime(&now);
    int GMTDay = ltm->tm_mday;
    int GMTYear = 1900 + ltm->tm_year;
    std::string GMTWeek, GMTMonth, GMTime, GMT;
    switch (ltm->tm_wday) {
    case 0: GMTWeek = "Sun"; break;
    case 1: GMTWeek = "Mon"; break;
    case 2: GMTWeek = "Tue"; break;
    case 3: GMTWeek = "Wed"; break;
    case 4: GMTWeek = "Thu"; break;
    case 5: GMTWeek = "Fri"; break;
    case 6: GMTWeek = "Sat"; break;
    }
    switch (ltm->tm_mon) {
    case 0: GMTMonth = "Jan"; break;
    case 1: GMTMonth = "Feb"; break;
    case 2: GMTMonth = "Mar"; break;
    case 3: GMTMonth = "Apr"; break;
    case 4: GMTMonth = "May"; break;
    case 5: GMTMonth = "Jun"; break;
    case 6: GMTMonth = "Jul"; break;
    case 7: GMTMonth = "Aug"; break;
    case 8: GMTMonth = "Sept"; break;
    case 9: GMTMonth = "Oct"; break;
    case 10: GMTMonth = "Nov"; break;
    case 11: GMTMonth = "Dec"; break;
    }
    GMTime = std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string(ltm->tm_sec);
    GMT = "Data:" + GMTWeek + ", " + std::to_string(GMTDay) + " " + GMTMonth + " " + std::to_string(GMTYear) + " " + GMTime + " GMT\r\n";
    return GMT;
}
std::string Filetype(std::string filename) {
    int index = filename.length();
    while(index>0) {
        if (filename[index] == '.' || index == 0)
            break;
        index--;
    }
    std::string suffix(filename, index, filename.length());
    if(suffix==".html")
        return "text/html";
    else if(suffix==".data")
        return "application/json";
    else if(suffix==".css")
        return "text/css";
    else if(suffix==".js")
        return "text/javascript";
    else if(suffix==".png")
        return "image/png";
    else if(suffix==".svg")
        return "image/svg";
    else if(suffix==".ico")
        return "image/x-icon";
    else
        return "text/plain";
}
void Httpprocess(std::string httphead,std::string *filename) {
    //if(httphead->find_first_of("GET")) {
        int beg = 5, end = 0;
        while (end < 100) {
            if (httphead[end + beg] == ' ')
                break;
            end++;
        }
        if(end==0)
            *filename = "index.html";
        else
            filename->assign(httphead, beg, end);
    //}
    //else if(httphead.find_first_of("POST")) 
}
void Successhead(std::string filename, struct Clientinfo *cli) {
    cli->httphead.clear();
    cli->httphead += "HTTP/1.1 200 OK\r\n";                       
    cli->httphead += "Constent_Charset:utf-8\r\n";                
    cli->httphead += "Content-Language:zh-CN\r\n";                
    cli->httphead += "Content-Type:";                             
    cli->httphead += Filetype(filename);
    cli->httphead += "\r\n";                                      
    cli->httphead += "Connection:Keep-alive\r\n";                 
    cli->httphead += "Content-Length:";                           
    cli->httphead += std::to_string(cli->remaining);
    cli->httphead += "\r\n";                                      
    //cli->httphead += "Cache-Control: no-cache max-age=0\r\n";
    cli->httphead += GMTime();
    cli->httphead += "Server:Gwc/0.5\r\n\r\n";

    cli->remaining += cli->httphead.length();
}
#endif
