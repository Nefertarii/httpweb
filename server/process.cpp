#include "process.h"

const std::string HTMLDIR = "/home/ftp_dir/Webserver/Blog/";

int ReadProcess::Read()
{
    int n = serv::HTTPread(cli->get()->sockfd, &(cli->get()->readbuf));
    if (n == 1)
    { //read success.
        cli->get()->httptype = Httptype(cli->get()->readbuf);
        cli->get()->status = HTTP_READ_OK;
        return 1;
    }
    else if (n == 0)
    { //read to large.
        cli->get()->errcode = SIZE_TO_LARGE;
        return -1;
    }
    else
    { //client close
        cli->get()->errcode = CLIENT_CLOSE;
        return -1;
    }
}
int ReadProcess::GETprocess()
{                                  //GETֻ���ڷ���ҳ���ļ�
    std::string filedir = HTMLDIR; //�������ļ���λ��
    std::string filename;
    filename = serv::Substr(cli->get()->readbuf, 5, 100, ' '); //GET begin for 5
    if (filename == "0")
    {
        filename = "index.html";
    }
    if (filename == "-1")
    {
        cli->get()->errcode = NOT_THIS_FILE;
        return -1;
    }
    filedir += filename;
    cli->get()->filename = filedir;
    //std::cout << "\n[" << cli->get()->filename << "]\n";
    int n = serv::Readfile(cli);
    if (n == 1)
    { //��ȡ�ɹ�
        cli->get()->httphead = Responehead(200, filedir, cli->get()->remaining);
        cli->get()->remaining += cli->get()->httphead.length();
        cli->get()->status = WRITE_FILE;
        return 1;
    }
    else
    { //open/stat����
        cli->get()->errcode = FILE_READ_FAIL;
        return -1;
    }
}
int ReadProcess::POSTprocess()
{
    //��ȡλ�� λ�ò�ͬ������ʽ��ͬ
    std::string location = serv::Substr(cli->get()->readbuf, 6, 100, ' '); //POST=6
    if (location == "-1")                                                  //to long
    {
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    if (POSTChoess(location) < 0) //post type error
    {
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    //��ȡ����.
    cli->get()->info = serv::Substr_Revers(cli->get()->readbuf, 200, '\n');
    if (cli->get()->info == "-1")
    { //���ݹ�����û������
        cli->get()->errcode = POST_INFO_ERROR;
        return -1;
    }
    cli->get()->status = Login;
    return 1;
}
int ReadProcess::POSTChoess(std::string method)
{
    if (method == "login")
        cli->get()->status = Login;
    else if (method == "reset")
        cli->get()->status = Reset;
    else if (method == "register")
        cli->get()->status = Register;
    else if (method == "vote_up")
        cli->get()->status = Vote_Up;
    else if (method == "vote_down")
        cli->get()->status = Vote_Down;
    else if (method == "comment")
        cli->get()->status = Comment;
    else if (method == "content")
        cli->get()->status = Content;
    else if (method == "readcount")
        cli->get()->status = Readcount;
    else
    {
        cli->get()->status = SNONE;
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    return 1;
}
int ReadProcess::POSTChoess(SERV_STATE method)
{
    switch (method)
    {
    case Login:
        cli->get()->Strstate();
        if (POSTLogin())
        {
            cli->get()->info = "{\"Name\":\"gwc\",\"Age\":\"20\",\"session\":\"success\"}";
            cli->get()->remaining = cli->get()->info.length();
            cli->get()->httphead = Responehead(200, "login.js",cli->get()->remaining);
            cli->get()->remaining += cli->get()->httphead.length();
            cli->get()->status = WRITE_INFO;
            return 1; //�ɹ�����
        }
        cli->get()->info = "{\"session\":\"fail\"}";
        cli->get()->httphead = Responehead(200, "login.js", cli->get()->remaining);
        cli->get()->remaining += cli->get()->httphead.length();
        cli->get()->errcode = Login_Fail;
        return -1; //ʧ�ܲ��� ���غ�ֱ�ӽ���д״̬
    case Reset:
        /* code */
        //POSTReset();
        break;
    case Register:
        /* code */
        //POSTRegister();
        break;
    case Vote_Up:
        /* code */
        //POSTVote();
        break;
    case Vote_Down:
        /* code */
        //POSTVote();
        break;
    case Comment:
        /* code */
        //POSTComment();
        break;
    case Content:
        /* code */
        //POSTContent();
        break;
    case Readcount:
        /* code */
        //POSTReadcount();
        break;
    default:
        /* code */
        cli->get()->errcode = POST_LOCATION_ERROR;
        return -1;
    }
    return -1;
}
int ReadProcess::POSTLogin()
{
    //�����ݽ��н�ȡ���ж�
    //�ɹ�����1 ���򷵻�-1
    std::string username = serv::Substr(cli->get()->info, 0, 50, '&');
    std::string password = serv::Substr_Revers(cli->get()->info, 20, '&');
    //�����ݿ��ȡ

    if (username == "123" && password == "123")
    {
        return 1;
    }
    /*
    else if (username.length() > MAX_USERNAME || password.length() > MAX_PASSWORD)
    {
        return -1;
    }
    else if (username.length() == 0 || password.length() == 0)
    {
        return -1;
    }
    */
    cli->get()->errcode = Login_Fail;
    cli->get()->status = SNONE;
    cli->get()->Strerror();
    return -1;
}
int ReadProcess::POSTRegister()
{
    return 1;
}
int WriteProcess::Writehead()
{
    int n = serv::HTTPwrite(cli->get()->httphead, cli->get()->sockfd);
    if (n > 1)
        return 2;
    if (n <= 0)
    {
        if (n == 0)
        {
            cli->get()->errcode = WRITE_AGAIN;
            return 0;
        }
        cli->get()->errcode = WRITE_FAIL;
        return -1;
    }
    cli->get()->httphead.clear();
    return 1;
}
int WriteProcess::Writefile()
{
    if (cli->get()->remaining > WRITE_BUF_SIZE)
    {
        int n = serv::Writefile(cli->get()->send, cli->get()->remaining, cli->get()->sockfd, cli->get()->filefd);
        if (n < 0)
        {
            cli->get()->errcode = WRITE_FAIL;
            return -1;
        }
        cli->get()->send += WRITE_BUF_SIZE;
        cli->get()->writetime += 1;
        cli->get()->remaining -= WRITE_BUF_SIZE;
        return 1;
    }
    else
    {
        int n = serv::Writefile(cli->get()->send, cli->get()->remaining, cli->get()->sockfd, cli->get()->filefd);
        if (n < 0)
        {
            cli->get()->errcode = WRITE_FAIL;
            return -1;
        }
        cli->get()->Reset();
        cli->get()->writetime += 1;
        cli->get()->status = WRITE_OK;
        return 1;
    }
}
int WriteProcess::Writeinfo()
{
    for (;;)
    {
        if (cli->get()->remaining > WRITE_BUF_SIZE)
        {
            int n = serv::HTTPwrite(cli->get()->info, cli->get()->sockfd);
            if (n <= 0)
            {
                if (n == 0)
                {
                    cli->get()->errcode = WRITE_AGAIN;
                    return 0;
                }
                cli->get()->errcode = WRITE_FAIL;
                return -1;
            }
            cli->get()->send += WRITE_BUF_SIZE;
            cli->get()->remaining -= WRITE_BUF_SIZE;
            cli->get()->writetime++;
        }
        else
        {
            int n = serv::HTTPwrite(cli->get()->info, cli->get()->sockfd);
            if (n <= 0)
            {
                if (n == 0)
                {
                    cli->get()->errcode = WRITE_AGAIN;
                    return 0;
                }
                cli->get()->errcode = WRITE_FAIL;
                return -1;
            }
            cli->get()->Reset();
            cli->get()->writetime += 1;
            cli->get()->status = WRITE_OK;
            return 1;
        }
    }
}

//������֤��
//����״̬�벻ͬд�벻ͬ��json
void Jsonprocess(int state, CLIENT *cli)
{
    //�ж� find name �ɹ� ����json
    if (state)
    {
        cli->get()->info = "{\"Name\":\"gwc\",\"Age\":\"20\",\"session\":\"success\"}";
    }
    //ʧ��
    else
    {
        cli->get()->info = "{\"session\":\"fail\"}";
    }
    cli->get()->remaining += cli->get()->info.length();
}