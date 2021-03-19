#include"servhead.h"
void start_server() {
    int listenfd, connectfd, socketfd;
    socklen_t clilen;
    struct sockaddr_in cliaddr;
    struct sockaddr_in servaddr;
    int epollfd, nfds;
    int maxi = 0;
    struct epoll_event event_now, events[20];
    std::string readbuf, requesttypes;
    std::string filetype, filename;
    epollfd = epoll_create(MAX_CLI);
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);//socket set Non blocking

    event_now.data.fd = listenfd;
    event_now.events = EPOLLIN | EPOLLET;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event_now);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, LISTEN_WAIT);

    std::cout << "Server start!" << std::endl;
    while(1) {
        nfds = epoll_wait(epollfd, events, MAXEVENTS, TIMEOUT);
        if(nfds==-1)
            err_sys("epoll wait error.");
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == listenfd) { //is listen new socket    accept
                clilen = sizeof(cliaddr);
                connectfd = Accept(listenfd, (sockaddr *)&cliaddr, &clilen);  
                std::cout << "\nAccept from " << inet_ntoa(cliaddr.sin_addr) << std::endl;

                event_now.data.fd = connectfd;
                event_now.events = EPOLLIN | EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, connectfd, &event_now);
            }



            else if(events[i].events & EPOLLIN) { //is connect user       read
                int n = Read(connectfd, readbuf);
                if (readbuf.find_first_of("GET") == 0) {
                    requesttypes = "GET";
                    filename = http_process(requesttypes, readbuf);
                    filetype = file_process(filename);
                    std::cout << "Read from:" << inet_ntoa(cliaddr.sin_addr)
                              << "\tNeed file:" << filename
                              << "\tfiletype:" << filetype << std::endl;
                    reset(readbuf);
                }
                /*else if (readbuf.find_first_of("POST") == 0) {
                    requesttypes = "POST";
                    std::cout << "Read from " << inet_ntoa(cliaddr.sin_addr) << std::endl;
                    filename = http_process(requesttypes, readbuf) ;

                }*/
                event_now.data.fd = connectfd;
                event_now.events = EPOLLOUT | EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_MOD, connectfd, &event_now);
            }



            else if (events[i].events & EPOLLOUT) { //have date need      send
                connectfd = events[i].data.fd;
                int filefd;
                size_t filesize;
                //filesize and filefd get in function "Read_file"
                Read_file(filename, filefd, filesize);
                if(filesize==0) {
                    Write(connectfd, "HTTP/1.1 404 Not_Found\r\n\r\n");
                    std::cout << "fail." << std::endl;
                }
                else {
                    std::cout << "Send:httphead... ";
                    send_httphead(connectfd, filesize, filetype);
                    std::cout << "success." << std::endl;
                    std::cout << "Send:" << filename << "... ";
                    sendfile(connectfd, filefd, 0, filesize);
                    reset(requesttypes);
                    reset(filetype);
                    reset(filename);
                    std::cout << "success." << std::endl;
                }
                event_now.data.fd = connectfd;
                event_now.events = EPOLLIN | EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_MOD, connectfd, &event_now);
            }
        }
    }

}
