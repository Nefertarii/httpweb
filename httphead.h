#ifndef HTTPHEAD_H_
#define HTTPHEAD_H_

#define StatusOK                              200 //200 读取成功且合法
#define StatusForbidden                       403 //403 禁止访问
#define StatusBadRequest                      400 //400 非法请求 
#define StatusUnauthorized                    401 //401 需要登录 登录失败 


enum Method {
    Login_Fail = -5,
    Reset_Fail = -4,
    Create_Fail = -3,
    ERROR = -1,
    GET = 1,
    POST = 2,
    Login_OK = 3,
    Reset_OK = 4,
    Create_OK = 5
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
        return "image/svg+xml";
    else if(suffix==".ico")
        return "image/x-icon";
    else
        return "text/plain";
}

//method == GET  info write file name
//method == POST info里写入读到的登录信息 httphead截取为请求的位置



//判断请求类型
Method Httpprocess(std::string *httphead) {
    if (httphead->find_first_of("GET") == 0) {
        return GET;
    }
    else if(httphead->find_first_of("POST") == 0) {
        return POST;
    }
    else {
        return ERROR;
    }
}

//返回处理得到的文件名
std::string GETprocess(std::string httphead) {
    int beg = 5, end = 0;
    while (end <= 100) { //读取要求的文件位置
        if (httphead[end + beg] == ' ')
            break;
        end++;
    }
    if(end == 0) { //默认返回index.html
        return "index.html";
    }
    else if(end <= 100) { //从头中截取文件名
        return httphead.substr(beg, end);
    }
    else { //文件地址要求过长
        return "ERROR";
    }
}


void Responehead(int state, std::string filename, Clientinfo *cli) {
    
    cli->httphead += "HTTP/1.1 ";
    cli->httphead += std::to_string(state);
    cli->httphead += Headstate(state);
    cli->httphead += "Constent_Charset:utf-8\r\n";                
    cli->httphead += "Content-Language:zh-CN\r\n";
    cli->httphead += "Connection:Keep-alive\r\n";
    cli->httphead += "Content-Type:";                             
    cli->httphead += Filetype(filename);
    cli->httphead += "\r\n";
    /*if(state == StatusOK) {                 //200
        if (cli->filefd > 0) {
            cli->httphead += "Content-Type:";                             
            cli->httphead += Filetype(info);
            cli->httphead += "\r\n";
        }
        else if (cli->bodyjson.length() > 0) {
            cli->httphead += "Content-Type:application/x-www-form-urlencoded\r\n";
        }
    }
    else {
        cli->httphead += "Content-Type:";                             
        cli->httphead += Filetype(info);
        cli->httphead += "\r\n";
    }*/
    cli->httphead += "Content-Length:";
    cli->httphead += std::to_string(cli->remaining);
    cli->httphead += "\r\n";
    //cli->httphead += "cache-control:no-cache\r\n";
    cli->httphead += GMTime();
    cli->httphead += "Server:Gwc/0.9 (C++)\r\n\r\n";
    

    cli->remaining += cli->httphead.length();
}
#endif
