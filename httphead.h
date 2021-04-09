#ifndef HTTPHEAD_H_
#define HTTPHEAD_H_

#define StatusContinue                        100
#define StatusSwitchingProtocols              101   
#define StatusOK                              200 //200 读取成功且合法
#define StatusCreated                         201
#define StatusAccepted                        202
#define StatusNonAuthoritativeInfo            203
#define StatusNoContent                       204
#define StatusResetContent                    205
#define StatusPartialContent                  206   
#define StatusMultipleChoices                 300
#define StatusMovedPermanently                301
#define StatusFound                           302
#define StatusSeeOther                        303
#define StatusNotModified                     304
#define StatusUseProxy                        305
#define StatusTemporaryRedirect               307 
#define StatusBadRequest                      400 //400 非法请求 
#define StatusUnauthorized                    401 //401 需要登录 登录失败
#define StatusPaymentRequired                 402
#define StatusForbidden                       403 //403 禁止访问 
#define StatusNotFound                        404  
#define StatusMethodNotAllowed                405
#define StatusNotAcceptable                   406
#define StatusProxyAuthRequired               407
#define StatusRequestTimeout                  408
#define StatusConflict                        409
#define StatusGone                            410
#define StatusLengthRequired                  411
#define StatusPreconditionFailed              412
#define StatusRequestEntityTooLarge           413
#define StatusRequestURITooLong               414
#define StatusUnsupportedMediaType            415
#define StatusRequestedRangeNotSatisfiable    416
#define StatusExpectationFailed               417
#define StatusTeapot                          418 
#define StatusInternalServerError             500 //500 服务器错误  
#define StatusNotImplemented                  501
#define StatusBadGateway                      502
#define StatusServiceUnavailable              503
#define StatusGatewayTimeout                  504
#define StatusHTTPVersionNotSupported         505   

// New HTTP status codes from RFC 6585. Not exported yet in Go 1.1.
// See discussion at https://codereview.appspot.com/7678043/
#define statusPreconditionRequired           = 428
#define statusTooManyRequests                = 429
#define statusRequestHeaderFieldsTooLarge    = 431
#define statusNetworkAuthenticationRequired  = 511

//#define FILE_ = 1
//#define JSON_ = 2




//httphead bodyjson remaining send filefd在每次写完成后处理
//session sockfd在关闭时处理
struct Clientinfo {
    std::string httphead;
    std::string bodyjson;
    int remaining;
    int send;
    int filefd;
    int sockfd;
    bool session;
    Clientinfo operator=(struct Clientinfo tmp_) {
        struct Clientinfo tmp;
        tmp.httphead = tmp_.httphead;
        tmp.bodyjson = tmp_.bodyjson;
        tmp.sockfd = tmp_.sockfd;
        tmp.remaining = tmp_.remaining;
        tmp.send = tmp_.send;
        tmp.filefd = tmp_.filefd;
        tmp.session = tmp_.session;
        return tmp;
    }
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

//请求类型为GET  info里写入文件名
//请求类型为POST info里写入读到的登录信息 httphead截取为请求的位置
std::string Httpprocess(std::string *httphead,std::string *info) {
    std::string DIR = "/home/ftp_dir/Webserver/Blog/";
    if (httphead->find_first_of("GET") == 0) {
        int beg = 5, end = 0;
        while (end < 100) {
            if (httphead[0][end + beg] == ' ')
                break;
            end++;
        }
        if(end == 0) {
            *info = DIR + "index.html";
        }
        else {
            *info = DIR + info->assign(*httphead, beg, end);
        }
        return "GET";
    }
    else if(httphead->find_first_of("POST") == 0) {   
        //获取数据
        int beg = 0, end = httphead->length();
        while (beg < 200) {
            if (httphead[0][end - beg] == '\n')
                break;
            beg++;
        }
        info->assign(*httphead, (end - beg + 1), beg);

        //获取登录的位置.
        beg = 6, end = 0;
        while (end < 100) {
            if (httphead[0][end + beg] == ' ')
                break;
            end++;
        }
        if(end==0) {
            return "ERROR";
        }
        else {
            httphead->assign(*httphead, beg, end);
        }
        return "POST";
    }
    return "ERROR";
}

//对传入的登录数据进行截取、判断
//成功返回1 否则返回-1
int Postprocess(std::string userinfo,std::string *username, std::string *password) {
    int beg = 9, end = 0;
    while (1) {
        if (userinfo[beg + end] == '&') {
            username->assign(userinfo, beg, end);
            break;
        }
        if (end == 51) {//最长读取50位名字/邮箱
            return -1;
        }
        end++;
    }
    beg = beg + end + 10;
    end = 0;
    while(1) {
        if (userinfo[beg + end] == '&') {
            password->assign(userinfo, beg, end);
            break;
        }
        if (end == 21) {//最长读取20位密码
            return -1;
        }
        end++;
    }
    return 1;
}

std::string Headstate(int state) {
    switch (state)
    {
    case StatusOK:                  //200
        return " OK\r\n";
    case StatusBadRequest:          //400
        return " Bad Request\r\n";
    case StatusUnauthorized:        //401
        return " Unauthorized\r\n";
    case StatusForbidden:           //403
        return " Forbidden\r\n";
    case StatusNotFound:            //404
        return " Not Found\r\n";
    default:
        return " \r\n";
    }
}

//state状态码 info根据需要传入文件名
void Responehead(int state, std::string info, Clientinfo *cli) {
    
    cli->httphead += "HTTP/1.1 ";
    cli->httphead += std::to_string(state);
    cli->httphead += Headstate(state);
    cli->httphead += "Constent_Charset:utf-8\r\n";                
    cli->httphead += "Content-Language:zh-CN\r\n";
    cli->httphead += "Connection:Keep-alive\r\n";
    cli->httphead += "Content-Type:";                             
    cli->httphead += Filetype(info);
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
    cli->httphead += "Server:Gwc/0.8 (C++)\r\n\r\n";
    

    cli->remaining += cli->httphead.length();
}
#endif
