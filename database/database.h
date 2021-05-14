#ifndef DATABASE_H_
#define DATABASE_H_

#include <chrono>
#include <iostream>
#include <list>
#include <mysqlx/xdevapi.h>
#include <string>
#include <time.h>
#include <typeinfo>

extern const int MYSQLPORT = 33060;
extern const std::string MYSQLUSERNAME = "root";
extern const std::string MYSQLPASSWORD = "GUOWUCHENG1";
extern const std::string IMAGEDIR = "/home/ftp_dir/Webserver/Blog/Image";
/**
 * INSERT INTO User_Data(User_name,User_create_time,User_email,User_password)
 * table User_Data: User_ID, User_image, User_name, User_create_time, User_email, User_password
 * table Comment: Comment_ID, Comment_main, Comment_time, User_ID
 * table Article: Article_ID, Article_main, Article_time, Comment_ID, User_ID
 * table Web_log: Log_time, Log_main, Log_state
 **/
//mysqlx::Table UserData = db.getTable("User_Data");
//mysqlx::RowResult result = UserData.select("User_ID", "User_name").execute();
//std::list<mysqlx::Row> rows = result.fetchAll();
class MySQLdata
{
private:
    mysqlx::Session session(MYSQLPORT, MYSQLUSERNAME, MYSQLPASSWORD);

public:
    MySQLdata();
    int Mysqllogin(std::string account_, std::string password_);
    int Mysqlregister(std::string username_, std::string useremail_, std::string password_);
    ~MySQLdata();
};
int MySQLdata::Mysqllogin(std::string account_, std::string password_)
{
    session.sql("USE User_Data");
    mysqlx::string password = password_;
    mysqlx::RowResult result;
    if (account_.find('@') == account_.npos) //use name login
    {
        result = session.sql("SELECT User_name,User_password "
                             "FROM User_Data "
                             "WHERE User_name = '" +
                             account_ + "'")
                     .execute();
    }
    else //use email login
    {
        result = session.sql("SELECT User_email,User_password "
                             "FROM User_Data "
                             "WHERE User_email = '" +
                             account_ + "'")
                     .execute();
    }
    // Gets the row and prints the column
    std::list<mysqlx::Row> rows = result.fetchAll();
    if (rows.size() >= 1)
    {
        for (auto it : rows)
        {
            std::cout << "Account:" << it[0] << "  User_password:" << it[1] << "\n";
            if (it[1] == password)
                return 1;
        }
        return 0;
    }
    return 0;
}
int MySQLdata::Mysqlregister(std::string username_, std::string useremail_, std::string password_)
{
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto value = now_ms.time_since_epoch();
    int duration2 = value.count();
    session.sql("USE User_Data");
    session.sql("INSERT INTO User_Data(User_name,User_create_time,User_email,User_password) "
                "VALUE('" +
                username_ + "',")
    //User_image=ID, User_create_time = unixtime
}

//...

#endif