#ifndef HTTPHEAD_H_
#define HTTPHEAD_H_

//httphead bodyjson remaining send filefd��ÿ��д��ɺ���
//session sockfd�ڹر�ʱ����
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
    Forbidden = 403,//Ȩ�޲���
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
        return "image/svg+xml";
    else if(suffix==".ico")
        return "image/x-icon";
    else
        return "text/plain";
}

//��������ΪGET  info��д���ļ���
//��������ΪPOST info��д������ĵ�¼��Ϣ
std::string Httpprocess(std::string httphead,std::string *info) {
    if (httphead.find_first_of("GET") == 0) {
        int beg = 5, end = 0;
        while (end < 100) {
            if (httphead[end + beg] == ' ')
                break;
            end++;
        }
        if(end==0) {
            *info = "index.html";
        }
        else {
            info->assign(httphead, beg, end);
        }
        return "GET";
    }
    else if(httphead.find_first_of("POST") == 0) {
        int beg = 0, end = httphead.length();
        while (beg < 200) {
            if (httphead[end - beg] == '\n')
                break;
            beg++;
        }
        info->assign(httphead, (end - beg + 1), beg);
        return "POST";
    }
    return "ERROR";
}
//����ɹ�httpͷ 200
void Successrequset(std::string filename, struct Clientinfo *cli) {
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
    cli->httphead += "Server:Gwc/0.7 (Centos)\r\n\r\n";

    cli->remaining += cli->httphead.length();
}
std::string Statecode(int state) {
    switch (state)
    {
    case 400:
        return " bad_request\r\n";
    case 403:
        return " forbidden\r\n";
    case 404:
        return " not_found\r\n";
    default:
        return " \r\n";
    }
}
//�����쳣httpͷ
void Badrequset(int state,struct Clientinfo *cli) {
    cli->httphead.clear();
    cli->httphead += "HTTP/1.1 ";
    cli->httphead += std::to_string(state);
    cli->httphead += Statecode(state);
    cli->httphead += "Constent_Charset:utf-8\r\n";        
    cli->httphead += "Content-Language:zh-CN\r\n";
    cli->httphead += "Content-Type:html/text\r\n";                                                              
    cli->httphead += "Connection:Keep-alive\r\n";                 
    cli->httphead += "Content-Length:";                           
    cli->httphead += std::to_string(cli->remaining);
    cli->httphead += "\r\n";    
    cli->httphead += GMTime();
    cli->httphead += "Server:Gwc/0.7 (Centos)\r\n\r\n";                                                                   
    //cli->httphead += "Cache-Control: no-cache max-age=0\r\n";

    cli->remaining += cli->httphead.length();
}
//��body�е����ݵĽ�ȡ���ж�
//�ɹ�����1 ���򷵻�-1
int Postprocess(std::string userinfo,std::string *username, std::string *password) {
    int beg = 9, end = 0;
    while (1) {
        if (userinfo[beg + end] == '&') {
            username->assign(userinfo, beg, end);
            break;
        }
        if (end == 51) {//���ȡ50λ����/����
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
        if (end == 21) {//���ȡ20λ����
            return -1;
        }
        end++;
    }
    return 1;
}

#endif
