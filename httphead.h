#ifndef HTTPHEAD_H_
#define HTTPHEAD_H_


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
std::list<std::string> responehead_html(int filesize,std::string filetype) {
    std::string state_line = "HTTP/1.1 200 OK\r\n";
    std::string constent_charset = "Constent_Charset:utf-8\r\n";
    std::string content_language = "Content-Language:zh-CN\r\n";   
    std::string content_type = "Content-Type:";
    content_type += filetype;
    content_type += "\r\n";
    std::string connection = "Connection:Keep-alive\r\n";
    std::string content_length = "Content-Length:";
    std::string cache_control = "Cache-Control: no-cache max-age=0\r\n";
    content_length += std::to_string(filesize);
    content_length += "\r\n";
    std::string date = GMTime();
    std::string server = "Server:Gwc/0.3\r\n\r\n";

    std::list<std::string > HTTPHEAD;
    HTTPHEAD.push_back(state_line);
    HTTPHEAD.push_back(constent_charset);
    HTTPHEAD.push_back(content_language);
    HTTPHEAD.push_back(content_type);
    HTTPHEAD.push_back(connection);
    HTTPHEAD.push_back(content_length);
    HTTPHEAD.push_back(cache_control);
    HTTPHEAD.push_back(date);
    HTTPHEAD.push_back(server);



    return HTTPHEAD;
}
std::string Httpprocess(std::string httphead,struct Clientinfo *cli) {
    



    if (httphead == "GET") {
        int beginindex = 5, endindex = 0;
        while (1) {
            if (readbuf[endindex + beginindex] == ' ')
                break;
            endindex++;
        }
        if(endindex==0)
            responfile = "index.html";
        else
            responfile.assign(readbuf, beginindex, endindex);
    }
    /*else if (requesttypes == "POST")
    {
        int beginindex = 6, endindex = 0;
        while (1) {
            if (readbuf[endindex + beginindex] == ' ')
                break;
            endindex++;
        }
        if(endindex==0)
            responfile = "index.html";
        else
            responfile.assign(readbuf, beginindex, endindex);
    }*/
    return responfile;
}
std::string file_process(std::string filename) {
    int index = filename.length(); 
    while(1) {
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

#endif
