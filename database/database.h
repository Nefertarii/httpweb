#ifndef DATABASE_H_
#define DATABASE_H_

#include <mysqlx/xdevapi.h>
#include <string>

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

//mysql operator
struct Mysqloperation
{
    int Mysqllogin(std::string account_, std::string password_);
    int Mysqlregister(std::string username_, std::string useremail_, std::string password_);
};

//...

#endif