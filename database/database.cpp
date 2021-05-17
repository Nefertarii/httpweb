#include "database.h"
#include <iostream>
#include <chrono>
#include <time.h>

static const int MYSQLPORT = 33060;
static const std::string MYSQLUSER = "root";
static const std::string MYSQLPASSWORD = "GUOWUCHENG1";
static mysqlx::Session mysession(MYSQLPORT, MYSQLUSER, MYSQLPASSWORD);

int Mysqloperation::Mysqllogin(std::string account_, std::string password_)
{
    mysession.sql("USE User_Data");
    mysqlx::string password = password_;
    mysqlx::RowResult result;
    if (account_.find('@') == account_.npos) //use name login
    {
        result = mysession.sql("SELECT User_name,User_password "
                               "FROM User_Data "
                               "WHERE User_name = '" +
                               account_ + "'")
                     .execute();
    }
    else //use email login
    {
        result = mysession.sql("SELECT User_email,User_password "
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
int Mysqloperation::Mysqlregister(std::string username_, std::string useremail_, std::string password_)
{
    auto now = std::chrono::system_clock::now();
    auto nowms = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto value = nowms.time_since_epoch();
    int nowtimeint = value.count();
    std::string nowtimestr = std::to_string(nowtimeint);
    mysession.sql("USE Web").execute();
    mysession.sql("INSERT INTO User_Data(User_name,User_create_time,User_email,User_password) "
                  "VALUE('" +
                  username_ + "'," + nowtimestr + ",'" + useremail_ + "','" + password_ + "')")
        .execute();
    return 1;
}
