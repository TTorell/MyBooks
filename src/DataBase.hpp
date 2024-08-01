/*
 * DataBase.hpp
 *
 *  Created on: 16 sep. 2020
 *      Author: torsten
 */

#ifndef DATABASE_HPP_
#define DATABASE_HPP_

#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <string>
#include <vector>

namespace MyBooks
{

class Book
{
  private:
    std::string _author;
    std::string _title;
    std::string _media_type;
    int _read_year;
    std::string _comment;

    friend std::ostream& operator <<(std::ostream& os, const Book& book);

  public:
    Book(const std::string& title = "unknown", const std::string& author = "unknown", const std::string& media_type = "book", int read_year = 0, const std::string& comment = "");
    std::string create_sql_insert_string() const;

    void set_author(const std::string& author)
    {
      _author = author;
    }
    ;
    void set_title(const std::string& title)
    {
      _title = title;
    }
    ;
    void set_media_type(const std::string& media_type)
    {
      _media_type = media_type;
    }
    ;
    void set_comment(const std::string& comment)
    {
      _comment = comment;
    }
    ;
    void set_read_year(int read_year)
    {
      _read_year = read_year;
    }
    ;

    bool check_fully_specified()
    {
        return this->_author != "unknown" && _author != "" &&
               this->_title != "unknown" && this->_title != "";
    }

};

class DataBase
{
  public:
    DataBase();
    virtual ~DataBase();
    DataBase& operator=(const DataBase& other);
    int open_database(const std::string& filename);
    void close_database();
    int create_table_Books();
    int fill_db_from_file(const std::string& filename);
    int save_db_to_file(const std::string& filename);
    std::string execute_sql_insert_or_create(const std::string& statement);
    std::string create_sql_select_statement(const std::string& title, const std::string& author,  bool read_year_on, const std::string& red_year);
    std::string execute_sql_select(const std::string& statement, std::vector<std::vector<std::string>>& result);
    void open_logfile(const std::string& logfilename);
    void close_logfile();
  private:
    sqlite3* _DB;
};

} /* namespace MyBooks */

#endif /* DATABASE_HPP_ */
