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

    WRITE_HEAD_FAIL = -9,
    WRITE_FILE_FAIL = -10,
    WRITE_INFO_FAIL = -11,
    FILE_READ_FAIL = -12,
    NOT_THIS_FILE = -13,
    POST_INFO_ERROR = -14,
    POST_LOCATION_ERROR = -15,
    SIZE_TO_LARGE = -16,
    CLIENT_CLOSE = -17,
    ERNEL_CACHE_FULL = -18,
    ERRORLAST = -(1 << 30)
};

enum SERV_STATE
{
    SNONE = 0,
    FAIL,
    WRITE_READY,
    WRITE_HEAD,
    WRITE_FILE,
    WRITE_INFO,
    WRITE_AGAIN,
    WRITE_FAIL,
    READ_FAIL,
    WRITE_OK,
    Login,
    Reset,
    Register,
    Vote_Up,
    Vote_Down,
    Comment,
    Content,
    Readcount,
    Verification,
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
    [-Register_Fail]            = "Repeat email address. ",
    [-Reset_Fail]               = "Reset account password fail. ",
    [-Login_Fail]               = "Error username or password. ",
    [-WRITE_HEAD_FAIL]          = "Write head fail please see system errno. ",
    [-WRITE_FILE_FAIL]          = "Write file fail please see system errno. ",
    [-WRITE_INFO_FAIL]          = "Write info fail please see system errno. ",
    [-FILE_READ_FAIL]           = "Read file fail please see system errno. ",
    [-NOT_THIS_FILE]            = "Not this file. ",
    [-POST_INFO_ERROR]          = "Post info too long or not set. ",
    [-POST_LOCATION_ERROR]      = "Post location error. ",
    [-SIZE_TO_LARGE]            = "Size to large. ",
    [-CLIENT_CLOSE]             = "Read/Write fail, client close. ",
    [-ERNEL_CACHE_FULL]         = "Ernel cache full do again. "
};

static const char *servstate_map[] =
{
    [SNONE]                    = "Undefine Server state. ",  
    [FAIL]                     = "Process fail",
    [WRITE_READY]              = "Ready to write. ",
    [WRITE_HEAD]               = "Send head now... ",
    [WRITE_FILE]               = "Send file now... ",
    [WRITE_INFO]               = "Send info now... ",
    [WRITE_AGAIN]              = "Write again:",
    [WRITE_FAIL]               = "Write fail:",
    [READ_FAIL]                = "Read fail:",
    [WRITE_OK]                 = "Write done.",
    [Login]                    = "Login",
    [Reset]                    = "Reset password",
    [Register]                 = "Register account",
    [Vote_Up]                  = "Vote up",
    [Vote_Down]                = "Vote down",
    [Comment]                  = "Comment",
    [Content]                  = "Content",
    [Readcount]                = "Readcount add",
    [Verification]             = "Verification"
};

#endif