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
    Create_Fail = -6,
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

enum HTTP_TYPE
{
    ERROR = -1,
    HNONE,
    GET,
    POST,
};

enum SERV_PROCESS
{
    SNONE = 0,
    Login,
    Reset,
    Create,
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
    WRITE_FILE_OK,
    Login_OK,
    Reset_OK,
    Create_OK,
    Vote_Up_OK,
    VOte_Down_OK,
    Comment_OK,
    Content_OK,
    Readcount_OK,
    STATELAST_ = (1 << 30)
};

const char *serverr_map[] =
{
    [-ENONE]                    = "Undefine Server error. ",
    [-Comment_Fail]             = "Comment fail. ",
    [-Content_Fail]             = "Content fail. ",
    [-Readcount_Fail]           = "Readcound add fail. ",
    [-Vote_Down_Fail]           = "Vote down fail. ",
    [-Vote_Up_Fail]             = "Vote up fail. ",
    [-Create_Fail]              = "Create account fail. ",
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

const char *servstate_map[] =
{
    [SNONE]                    = "Undefine Server state error",  
    [Login]                    = "Now login...",
    [Reset]                    = "Now reset password...",
    [Create]                   = "Now create account...",
    [Vote_Up]                  = "Now vote up...",
    [Vote_Down]                = "Now vote down...",
    [Comment]                  = "Now comment...",
    [Content]                  = "Now content...",
    [Readcount]                = "Readcount add..."
};
#endif