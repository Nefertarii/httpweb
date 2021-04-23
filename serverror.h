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
    WRITE_FILE_FAIL = -10,
    FILE_READ_FAIL = -11,
    NOT_THIS_FILE = -12,
    POST_INFO_ERROR = -13,
    POST_LOCATION_ERROR = -14,
    SIZE_TO_LARGE = -15,
    CLIENT_CLOSE = -16,

    ERRORLAST = (1 << 31),
}

enum HTTP_TYPE
{
    ERROR = -1,
    HNONE,
    GET,
    POST,
}

enum SERV_PROCESS
{
    PNONE = 0,
    HTTP_READ_OK,
    FILE_READ_OK,
    POST_INFO_OK,
    POST_LOCATION_OK,
    WRITE_OK,
    WRITE_FILE_OK,
    Login,
    Login_OK,
    Reset,
    Reset_OK,
    Create,
    Create_OK,
    Vote_Up,
    Vote_Up_OK,
    Vote_Down,
    VOte_Down_OK,
    Comment,
    Comment_OK,
    Content,
    Content_OK,
    Readcount,
    Readcount_OK
};

const char *servcode_map[] =
{
    [-ENONE]                    = "See server error code",
    [-Comment_Fail]             = "Comment fail",
    [-Content_Fail]             = "Content fail",
    [-Readcount_Fail]           = "Readcound add fail",
    [-Vote_Down_Fail]           = "Vote down fail",
    [-Vote_Up_Fail]             = "Vote up fail",
    [-Create_Fail]              = "Create account fail",
    [-Reset_Fail]               = "Reset account password fail",
    [-Login_Fail]               = "Login fail",
    [-WRITE_FAIL]               = "Write fail please see errno",
    [-WRITE_FILE_FAIL]          = "Write file fail please see errno",
    [-FILE_READ_FAIL]           = "Read file fail please see errno",
    [-POST_INFO_ERROR]          = "Post info too long or not set",
    [-POST_LOCATION_ERROR]      = "Post location error",
    [-SIZE_TO_LARGE]            = "Read size to large",
    [-CLIENT_CLOSE]             = "Read fail, client close"
};

#endif