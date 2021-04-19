#ifndef SERVHEAD_H_
#define SERVHEAD_H_

#define OPEN_MAX 64
#define READ_BUF_SIZE 8*1024
#define WRITE_BUF_SIZE 2*1024
#define SERV_PORT 80
#define MAXEVENTS 50
#define TIMEOUT 0
#define MAX_CLI 50      //maximum number of client connections
#define LISTEN_WAIT 20
#define INFTIM (-1)
extern const std::string DIR = "/home/ftp_dir/Webserver/Blog/";
extern const std::string PAGE400 = "/home/ftp_dir/Webserver/Blog/Errorpage/Page400.html";
extern const std::string PAGE401 = "/home/ftp_dir/Webserver/Blog/Errorpage/Page401.html";
extern const std::string PAGE403 = "/home/ftp_dir/Webserver/Blog/Errorpage/Page403.html";
extern const std::string PAGE404 = "/home/ftp_dir/Webserver/Blog/Errorpage/Page404.html";

int Socket(int family, int type, int protocol);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Listen(int fd, int backlog);
int Accept(int listenfd);
void err_sys(const char *fmt) {
    std::cout << fmt << strerror(errno) << std::endl;
}
//void Connect(int fd, const struct sockaddr *sa, socklen_t salen);


//����������sockfd ������д��str��  
//�ɹ����ض�ȡ���ֽ���  ʧ�ܷ���0/-1
int HTTPread(int sockfd, std::string *str) { 
    char tmp_readbuf[READ_BUF_SIZE] = {0};
    int readsize = read(sockfd, tmp_readbuf, READ_BUF_SIZE);
    if (readsize < 0) {
        if(errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
            err_sys("Read error:");
            //Closeclient(cli); ������
            return -1;
        }    
    }
    else if (readsize == 0) {
        std::cout << "Client close.\n";
        //Closeclient(cli); �Զ˹ر� FIN
        return -1;
    }
    else if (readsize > READ_BUF_SIZE) {
        return 0;
    }
    *str = tmp_readbuf;
    return 1;
}
//����Ҫ������ļ�����(��������Ŀ¼��������ִ����·��) 
//�ɹ�����1 cli��д���ȡ�ı����ļ�fd�������ļ���С����ʣ�෢�͵Ĵ�С
//ʧ�ܷ���0/-1
int Readfile(std::string filename, CLIENT *cli) {
    struct stat filestat_;
    int filestat;
    const char *tmp_char = filename.c_str();
    cli->get()->filefd = open(tmp_char, O_RDONLY);
    if(cli->get()->filefd < 0) {
        err_sys("func(Readfile) open error:");
        return 0; 
    }

    filestat = stat(tmp_char, &filestat_);
    if(filestat < 0) {
        err_sys("func(Readfile) stat error:");
        return -1;
    }
    cli->get()->remaining += filestat_.st_size;
    return 1;
}
//���䱾��info�е�����
//�ɹ�����1 ʧ�ܷ���0/-1  0=дδ��� ��Ҫ�ٴ�ִ��   -1=���� ��ر�����
int HTTPwrite(std::string info,int sockfd) {
    const char *tmp_char = info.c_str();
    int n = 0;//��¼�ź��������� ��ֹ��ס
    while(1) {
        if(n > 10)
            return 0;
        if (write(sockfd, tmp_char, strlen(tmp_char)) < 0) {
            if(errno == EINTR) {
                std::cout << "signal interruption." << std::endl;
                n++;
                continue;//signal interruption
            }
            else if(errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0;//kernel cache full
            }
            else {
                err_sys("Write error:");
                return -1;//another error
            }
        }      
        return 1;
    }
}
//��cli�ж�ȡ����ı����ļ�fd ����ϵͳ����sendfile��ȡ�����͸��ļ�
//�ɹ�����1 ʧ�ܷ���0/-1   0=дδ��� ��Ҫ�ٴ�ִ��   -1=���� ��ر�����
int Writefile(off_t offset, int remaining, int sockfd, int filefd) {
    int num = 0;
    while (1) {
        if(num > 10)
            return 0;
        int n = sendfile(sockfd, filefd, &offset, WRITE_BUF_SIZE);
        if (n < 0) {
            if (errno == EINTR) {
                std::cout << "signal interruption." << std::endl;
                num++;
                continue;
            }
            else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0;
            }
            else {
                err_sys("Write error:");
                return -1;
            }
        }
        return 1;
    }
}



int Socket(int family, int type, int protocol) {
    int sockfd = socket(family, type, protocol);
    if (sockfd < 0) {
        err_sys("Socket error:");
        exit(errno);
    }
    return sockfd;
}
void Bind(int fd, const struct sockaddr *sa, socklen_t salen) {
	if (bind(fd, sa, salen) < 0) {
		err_sys("Bind error:");
        exit(errno);
    }
}
void Listen(int fd, int backlog) {
	if (listen(fd, backlog) < 0) {
		err_sys("Listen error:");
        exit(errno);
    }
}
int Accept(int listenfd) {
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen = sizeof(cliaddr);
    while(1) {
        int connectfd = accept(listenfd, (sockaddr *)&cliaddr, &cliaddrlen);
        if (connectfd < 0) {
            if(errno == EINTR)
                continue;
            err_sys("Accept error:");
            exit(errno);
        }
        std::cout << "Get accept form:" << inet_ntoa(cliaddr.sin_addr) << std::endl;
        return connectfd;
    }
    
}
void Close(int fd) {
    if (close(fd) < 0)
        err_sys("Close error:");
}
void Setnoblocking(int fd) { //������
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag >= 0) {
        fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    }
}
void Setreuseaddr(int fd) { //���õ�ַ ����FIN_WAIT״̬
    int reuseaddr = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) < 0)
        err_sys("setsockopt(SO_REUSEADDR) failed");
}
void Setbuffer(int fd) { //���û���
    int RecvBuf = READ_BUF_SIZE; 
    int SendBuf = WRITE_BUF_SIZE;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char *)&RecvBuf, sizeof(int)) < 0) {
        err_sys("setsockopt(SO_RCVBUF) error:");
    }
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char *)&SendBuf, sizeof(int)) < 0) {
        err_sys("setsockopt(SO_SNDBUF) error:");
    }
}
int Ramdom() {
    std::random_device rd;
    std::mt19937 mt(rd());
    return mt() % 100;
}



#endif
