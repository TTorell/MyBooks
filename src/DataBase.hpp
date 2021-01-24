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

using namespace std;

namespace MyBooks
{

class Book
{
  private:
    string _author;
    string _title;
    string _media_type;
    int _read_year;
    string _comment;

  public:
    Book(const string& title = "unknown", const string& author = "unknown", const string& media_type = "book", int read_year = 0, const string& comment = "");
    string create_sql_insert_string();

    void set_author(const string& author)
    {
      _author = author;
    }
    ;
    void set_title(const string& title)
    {
      _title = title;
    }
    ;
    void set_media_type(const string& media_type)
    {
      _media_type = media_type;
    }
    ;
    void set_comment(const string& comment)
    {
      _comment = comment;
    }
    ;
    void set_read_year(int read_year)
    {
      _read_year = read_year;
    }
    ;

};

class DataBase
{
  public:
    DataBase();
    virtual ~DataBase();
    DataBase& operator=(const DataBase& other);
    int open_database(const string& filename);
    void close_database();
    int create_table_Books();
    int fill_db_from_file(const string& filename);
    int save_db_to_file(const string& filename);
    string execute_sql_insert_or_create(const string& statement);
    string create_sql_select_statement(const string& title, const string& author,  bool read_year_on, const string& red_year);
    string execute_sql_select(const string& statement, vector<vector<string>>& result);
    void open_logfile(const string& logfilename);
    void close_logfile();
  private:
    sqlite3* _DB;
};

} /* namespace MyBooks */

#endif /* DATABASE_HPP_ */
