#include "database.h"
#include <chrono>
#include <iostream>
#include <time.h>

static const int MYSQLPORT = 33060;
static const std::string MYSQLUSER = "root";
static const std::string MYSQLPASSWORD = "GUOWUCHENG1";

//判断使用什么方式登录并验证
//成功返回1 失败返回-1
std::string Mysqloperation::Mysqllogin(std::string account_, std::string password_)
{
    try
    {
        mysqlx::Session mysession(MYSQLPORT, MYSQLUSER, MYSQLPASSWORD);
        try
        {
            mysession.sql("USE Web").execute();
            mysqlx::string password = password_;
            mysqlx::RowResult result;
            if (account_.find('@') == account_.npos) //use name login
            {
                try
                {
                    result = mysession.sql("SELECT User_name,User_password,User_ID "
                                           "FROM User_Data "
                                           "WHERE User_name = '" +
                                           account_ + "'")
                                 .execute();
                }
                catch (const mysqlx::Error &err)
                {
                    std::cerr << "\nMySQL Error\n"
                              << err << '\n';
                    return "error";
                }
            }
            else //use email login
            {
                try
                {
                    result = mysession.sql("SELECT User_email,User_password,User_ID "
                                           "FROM User_Data "
                                           "WHERE User_email = '" +
                                           account_ + "'")
                                 .execute();
                }
                catch (const mysqlx::Error &err)
                {
                    std::cerr << "\nMySQL Error\n"
                              << err << '\n';
                    return "error";
                }
            }
            std::list<mysqlx::Row> rows = result.fetchAll();
            if (rows.size() >= 1)
            {
                for (auto it : rows)
                {
                    if (it[1] == password)
                    {
                        try
                        {
                            int userid_ = it[2].get<int>();
                            std::string userid = std::to_string(userid_);
                            result = mysession.sql("SELECT User_image "
                                                   "FROM User_Data "
                                                   "WHERE User_ID = '" +
                                                   userid + "'")
                                         .execute();
                        }
                        catch (const mysqlx::Error &err)
                        {
                            std::cerr << "\nMySQL Error\n"
                                      << err << '\n';
                            return "error1";
                        }
                        std::list<mysqlx::Row> rows = result.fetchAll();
                        for (auto it : rows)
                        {
                            std::string image = it[0].get<std::string>();
                            return image;
                        }
                    }
                }
                return "error";
            }
            return "error";
        }
        catch (const mysqlx::Error &err)
        {
            std::cerr << "\nMySQL Error\n"
                      << err << '\n';
            return "error";
        }
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "\nMySQL Error\n"
                  << err << '\n';
        return "error";
    }
}
int Mysqloperation::Mysqlregister(std::string username_, std::string useremail_, std::string password_)
{
    auto now = std::chrono::system_clock::now();
    auto nowms = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto value = nowms.time_since_epoch();
    int nowtimeint = value.count();
    std::string nowtimestr = std::to_string(nowtimeint);
    try
    {
        mysqlx::Session mysession(MYSQLPORT, MYSQLUSER, MYSQLPASSWORD);
        try
        {
            mysession.sql("USE Web").execute();
            try
            {
                mysession.sql("INSERT INTO User_Data(User_name,User_create_time,User_email,User_password) "
                              "VALUE('" +
                              username_ + "'," + nowtimestr + ",'" + useremail_ + "','" + password_ + "')")
                    .execute();
            }
            catch (const mysqlx::Error &err)
            {
                std::cerr << "\nMySQL Error\n";
                std::cerr << "Repeat info. " << err << '\n';
                return -1;
            }
        }
        catch (const mysqlx::Error &err)
        {
            std::cerr << "\nMySQL Error\n";
            std::cerr << "not this database. " << err << '\n';
            return -1;
        }
        return 1;
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "\nMySQL Error\n";
        std::cerr << "databases connect fail. " << err << '\n';
        return -1;
    }
}
