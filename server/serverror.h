#ifndef SERVERROR_H_
#define SERVERROR_H_

enum SERV_ERR
{
    ENONE = 0,
    Comment_Fail = -1,
    Content_Fail = -2,
    Readcount_Fail = -3,
    Vote_Down_Fail = -4,
    Vote_Up_Fail = -5,
    Register_Fail = -6,
    Reset_Fail = -7,
    Login_Fail = -8,
    WRITE_FAIL = -9,    

    WRITE_HEAD_FAIL = -10,
    WRITE_FILE_FAIL = -11,
    WRITE_INFO_FAIL = -12,
    FILE_READ_FAIL = -13,
    NOT_THIS_FILE = -14,
    POST_INFO_ERROR = -15,
    POST_LOCATION_ERROR = -16,
    SIZE_TO_LARGE = -17,
    CLIENT_CLOSE = -18,
    WRITE_AGAIN = -19,

    ERRORLAST = -(1 << 30),
};

enum SERV_STATE
{
    SNONE = 0,
    WRITE_READY,
    WRITE_HEAD,
    WRITE_FILE,
    WRITE_INFO,
    Login,
    Reset,
    Register,
    Vote_Up,
    Vote_Down,
    Comment,
    Content,
    Readcount,
    HTTP_READ_OK,
    FILE_READ_OK,
    POST_INFO_OK,
    POST_LOCATION_OK,
    WRITE_OK,
    Login_OK,
    Reset_OK,
    Register_OK,
    Vote_Up_OK,
    Vote_Down_OK,
    Comment_OK,
    Content_OK,
    Readcount_OK,
    STATELAST = (1 << 30)
};

static const char *serverr_map[] =
{
    [-ENONE]                    = "Undefine Server error. ",
    [-Comment_Fail]             = "Comment fail. ",
    [-Content_Fail]             = "Content fail. ",
    [-Readcount_Fail]           = "Readcound add fail. ",
    [-Vote_Down_Fail]           = "Vote down fail. ",
    [-Vote_Up_Fail]             = "Vote up fail. ",
    [-Register_Fail]            = "Register account fail. ",
    [-Reset_Fail]               = "Reset account password fail. ",
    [-Login_Fail]               = "Login fail. ",
    [-WRITE_FAIL]               = "Write fail please see system errno. ",
    [-WRITE_HEAD_FAIL]          = "Write head fail please see system errno. ",
    [-WRITE_FILE_FAIL]          = "Write file fail please see system errno. ",
    [-WRITE_INFO_FAIL]          = "Write info fail please see system errno. ",
    [-FILE_READ_FAIL]           = "Read file fail please see system errno. ",
    [-NOT_THIS_FILE]            = "Not this file. ",
    [-POST_INFO_ERROR]          = "Post info too long or not set. ",
    [-POST_LOCATION_ERROR]      = "Post location error. ",
    [-SIZE_TO_LARGE]            = "Read size to large. ",
    [-CLIENT_CLOSE]             = "Read/Write fail, client close. ",
    [-WRITE_AGAIN]              = "Kernel cache full do again. "
};

static const char *servstate_map[] =
{
    [SNONE]                    = "Undefine Server state. ",  
    [WRITE_READY]              = "Ready to write. ",
    [WRITE_HEAD]               = "Send head now... ",
    [WRITE_FILE]               = "Send file now... ",
    [WRITE_INFO]               = "Send info now... ",
    [Login]                    = "Now login... ",
    [Reset]                    = "Now reset password... ",
    [Register]                 = "Now register account... ",
    [Vote_Up]                  = "Now vote up... ",
    [Vote_Down]                = "Now vote down... ",
    [Comment]                  = "Now comment... ",
    [Content]                  = "Now content... ",
    [Readcount]                = "Readcount add... "
};

#endif