#include "servhead.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <random>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>

Clientinfo::Clientinfo()
{
    readbuf = "none";
    httphead = "none";
    info = "none";
    filename = "none";
    status = SNONE;
    errcode = ENONE;
    httptype = TNONE;
    writetime = 0;
    remaining = 0;
    bodylength = 0;
    send = 0;
    filefd = 0;
    sockfd = -1;
    session = false;
}
Clientinfo::Clientinfo(const Clientinfo &tmp)
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
    bodylength = tmp.bodylength;
    send = tmp.send;
    filefd = tmp.filefd;
    sockfd = tmp.sockfd;
    session = tmp.session;
}
Clientinfo &Clientinfo::operator=(struct Clientinfo &&tmp)
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
    bodylength = tmp.bodylength;
    send = tmp.send;
    filefd = tmp.filefd;
    sockfd = tmp.sockfd;
    session = tmp.session;
    return *this;
}
void Clientinfo::Reset()
{
    readbuf.clear();
    httphead.clear();
    info.clear();
    status = SNONE;
    errcode = ENONE;
    httptype = TNONE;
    writetime = 0;
    remaining = 0;
    bodylength = 0;
    send = 0;
    filefd = -1;
}
const char *Clientinfo::Strerror(int codenum)
{
    //std::cout << serverr_map[-codenum % -ERRORLAST];
    return serverr_map[-codenum % -ERRORLAST];
}
const char *Clientinfo::Strerror()
{
    //std::cout << serverr_map[-errcode % -ERRORLAST];
    return serverr_map[-errcode % -ERRORLAST];
}
const char *Clientinfo::Strstate(int codenum)
{
    //std::cout << servstate_map[codenum % -STATELAST];
    return servstate_map[codenum % STATELAST];
}
const char *Clientinfo::Strstate()
{
    //std::cout << servstate_map[status % -STATELAST];
    return servstate_map[status % STATELAST];
}

int serv::Socket(int family, int type, int protocol)
{
    int sockfd = socket(family, type, protocol);
    if (sockfd < 0)
    {
        serv::Sys_err("Socket error:");
        exit(errno);
    }
    return sockfd;
}
void serv::Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if (bind(fd, sa, salen) < 0)
    {
        serv::Sys_err("Bind error:");
        exit(errno);
    }
}
void serv::Listen(int fd, int backlog)
{
    if (listen(fd, backlog) < 0)
    {
        serv::Sys_err("Listen error:");
        exit(errno);
    }
}
int serv::Accept(int listenfd)
{
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen = sizeof(cliaddr);
    while (1)
    {
        int connectfd = accept(listenfd, (sockaddr *)&cliaddr, &cliaddrlen);
        if (connectfd < 0)
        {
            if (errno == EINTR)
                continue;
            serv::Sys_err("Accept error:");
            exit(errno);
        }
        std::cout << "Get accept form:" << inet_ntoa(cliaddr.sin_addr) << std::endl;
        return connectfd;
    }
}
void serv::Sys_err(const char *fmt)
{
    std::cout << fmt << std::strerror(errno) << std::endl;
}
void serv::Close(int fd)
{
    if (close(fd) < 0)
        serv::Sys_err("Close error:");
}
void serv::Setnoblocking(int fd)
{ //??????
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag >= 0)
    {
        fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    }
}
void serv::Setreuseaddr(int fd)
{ //???????? ????FIN_WAIT????
    int reuseaddr = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) < 0)
        serv::Sys_err("setsockopt(SO_REUSEADDR) failed");
}
void serv::Setbuffer(int fd)
{ //????????
    int RecvBuf = READ_BUF_SIZE;
    int SendBuf = WRITE_BUF_SIZE;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char *)&RecvBuf, sizeof(int)) < 0)
    {
        serv::Sys_err("setsockopt(SO_RCVBUF) error:");
    }
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char *)&SendBuf, sizeof(int)) < 0)
    {
        serv::Sys_err("setsockopt(SO_SNDBUF) error:");
    }
}
int serv::Random(int num)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    return mt() % num;
}
HTTP_TYPE serv::HTTPtype(std::string httphead)
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
//????????????sockfd ??????????str??
//????????????????????  ????????0/-1
int serv::HTTPread(int sockfd, std::string *str)
{
    char tmp_readbuf[READ_BUF_SIZE] = {0};
    int readsize = read(sockfd, tmp_readbuf, READ_BUF_SIZE);
    if (readsize < 0)
    {
        if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            serv::Sys_err("Read error:");
            //Closeclient(cli); ??????
            return -1;
        }
    }
    else if (readsize == 0)
    {
        //Closeclient(cli); ???????? FIN
        return -1;
    }
    else if (readsize > READ_BUF_SIZE)
    {
        return 0;
    }
    *str = tmp_readbuf;
    return 1;
}
//????????????????????(????????????????????????????????)
//????????1 cli????????????????????fd????????????????????????????????
//????????0/-1
int serv::Readfile(CLIENT *cli)
{
    struct stat filestat_;
    int filestat;
    const char *tmp_char = cli->get()->filename.c_str();
    cli->get()->filefd = open(tmp_char, O_RDONLY);
    if (cli->get()->filefd < 0)
    {
        std::cout << tmp_char << std::endl;
        serv::Sys_err("func(Readfile) error:");
        return 0;
    }
    filestat = stat(tmp_char, &filestat_);
    if (filestat < 0)
    {
        serv::Sys_err("func(Readfile) error:");
        return -1;
    }
    cli->get()->remaining += filestat_.st_size;
    cli->get()->bodylength += filestat_.st_size;
    return 1;
}
//????????????
//????????1 ????????0/-1  0=???????? ????????????   -1=???? ??????????
int serv::HTTPwrite(std::string info, int sockfd)
{
    if (info.length() == 0)
        return 2;
    const char *tmp_char = info.c_str();
    int num = 0; //???????????????? ????????
AGAIN:
    if (num > 10)
        return 0;
    if (write(sockfd, tmp_char, strlen(tmp_char)) < 0)
    {
        if (errno == EINTR)
        {
            std::cout << "signal interruption." << std::endl;
            num++;
            goto AGAIN; //signal interruption
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return 0; //kernel cache full
        }
        else
        {
            serv::Sys_err("Write error:");
            return -1; //another error
        }
    }
    return 1;
}
//????????????sendfile??????????
//????????1 ????????0/-1   0=???????? ????????????   -1=???? ??????????
int serv::Writefile(off_t offset, int sockfd, int filefd)
{
    int num = 0;
AGAIN:
    if (num > 10)
        return 0;
    int n = sendfile(sockfd, filefd, &offset, WRITE_BUF_SIZE);
    if (n < 0)
    {
        if (errno == EINTR)
        {
            std::cout << "signal interruption." << std::endl;
            num++;
            goto AGAIN;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return 0;
        }
        else
        {
            serv::Sys_err("Write error:");
            return -1;
        }
    }
    return 1;
}

std::string serv::Substr(std::string readbuf, int beg_i, int maxlength, char end_c)
{
    int beg = beg_i, end = 0;
    while (end <= maxlength)
    { //found end_c set index;
        if (readbuf[end + beg] == end_c)
            break;
        end++;
    }
    if (end == 0)
    { //not found
        return "0";
    }
    else if (end <= maxlength)
    { //return substring
        return readbuf.substr(beg, end);
    }
    else
    { //length > maxlength
        return "-1";
    }
}
std::string serv::Substr_Revers(std::string readbuf, int maxlength, char end_c)
{
    int beg = readbuf.length(), end = 0;
    while (end <= maxlength)
    {
        if (readbuf[beg - end] == end_c)
            break;
        end++;
    }
    if (end == 0)
    {
        return "0";
    }
    else if (end <= maxlength)
    {
        return readbuf.substr(beg - end + 1, end);
    }
    else
    {
        return "-1";
    }
}

int serv::Get_all_files(const std::string dir_, std::vector<std::string> &files)
{
    std::string dir = dir_;
    struct stat s;
    stat(dir.c_str(), &s);
    if (!S_ISDIR(s.st_mode))
    {
        std::cerr << "Protected verification dir\n";
        exit(1);
    }
    DIR *open_dir = opendir(dir.c_str());
    if (open_dir == nullptr)
    {
        std::cerr << "Empty verification dir\n";
        exit(1);
    }
    dirent *p = nullptr;
    while ((p = readdir(open_dir)) != nullptr)
    {
        struct stat st;
        if (p->d_name[0] != '.')
        {
            std::string filedir = dir + std::string("/") + std::string(p->d_name);
            stat(filedir.c_str(), &st);
            if (S_ISDIR(st.st_mode))
            {
                Get_all_files(filedir, files);
            }
            else if (S_ISREG(st.st_mode))
            {
                std::string filename = p->d_name;
                filename = filename.substr(0, 4);
                files.push_back(filename);
            }
        }
    }
    closedir(open_dir);
    return 1;
}
