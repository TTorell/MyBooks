/*
 * DataBase.cpp
 *
 *  Created on: 16 sep. 2020
 *      Author: torsten
 */

#include "DataBase.hpp"
#include <cstring>
#include <regex>

using namespace std::string_literals;
// File-private functions
namespace
{

// My database is UTF-8 encoded.
// So we need a convert-function
std::string iso_8859_1_to_utf8(const std::string& str)
{
  std::string str_out;
  for (uint8_t ch : str)
  {
    if (ch < 0x80)
    {
      str_out.push_back(ch);
    }
    else
    {
      str_out.push_back(0xc0 | ch >> 6);
      str_out.push_back(0x80 | (ch & 0x3f));
    }
  }
  return str_out;
}

std::string iso_8859_1_to_utf8(const char* c_string)
{
  std::string str_out(c_string);
  return iso_8859_1_to_utf8(str_out);
}

// match a regular expression in a string
bool regexp_match(const std::string& s, const std::string& regexp_string)
{
  std::smatch m;
  std::regex r(regexp_string);
  return (std::regex_match(s, m, r));
}

// replace TAB-characters with one space-character
std::string replace_tab_with_space(const std::string& s)
{
  return std::regex_replace(s, std::regex("\\t"), std::string(" "));
}

//// replace TAB-characters with space-characters
//string replace_tab_with_spaces(const string& s, int tab_width)
//{
//  string result;
//  int next_tab_stop;
//  int current_col = 0;
//  for (const char& c : s)
//  {
//    if (c != '\t')
//    {
//      result += c;
//      current_col++;
//      continue;
//    }
//    next_tab_stop = ((current_col + tab_width) / tab_width) * tab_width;
//    while (current_col < next_tab_stop)
//    {
//      result += ' ';
//      current_col++;
//    }
//  }
//  return result;
//}

// Remove leading space characters from string
std::string ltrim(const std::string& s)
{
  return std::regex_replace(s, std::regex("^\\s+"), std::string(""));
}

// Remove trailing space characters from string
std::string rtrim(const std::string& s)
{
  return std::regex_replace(s, std::regex("\\s+$"), std::string(""));
}

std::string trim(const std::string& s)
{
  return ltrim(rtrim(s));
}

// Remove windows/dos-like line-feeds (carriage return)
std::string remove_cr(const std::string& s)
{
  return std::regex_replace(s, std::regex("\\r$"), std::string(""));
}
} // End namespace

namespace MyBooks
{

std::ostream& operator <<(std::ostream& os, const Book& book)
{
  os << "-----------------------------------------------------" << std::endl;
  os << "title:      " << book._title << std::endl;
  os << "author:     " << book._author << std::endl;
  os << "read year:  " << book._read_year << std::endl;
  os << "media_type: " << book._media_type << std::endl;
  os << "comment:    " << book._comment << std::endl;
  os << "SQL: " << book.create_sql_insert_string() << std::endl;
  os << "-----------------------------------------------------" << std::endl;
  return os;
}

Book::Book(const std::string& title, const std::string& author, const std::string& media_type, int read_year, const std::string& comment) :
    _author(author), _title(title), _media_type(media_type), _read_year(read_year), _comment(comment)
{
}

std::string Book::create_sql_insert_string() const
{
  auto s = "hej"s;
  std::string statement = iso_8859_1_to_utf8("INSERT INTO Books (Author, Title, Media_Type, Read_Year,Comment) VALUES (\""s);
  statement += _author;
  statement += iso_8859_1_to_utf8("\", \""s);
  statement += _title;
  statement += iso_8859_1_to_utf8("\",\""s + _media_type + "\",\""s);
  statement += iso_8859_1_to_utf8(std::to_string(_read_year) + "\",\"");
  statement += _comment;
  statement += iso_8859_1_to_utf8("\")");
  // My database uses UFT-8 character encoding so;
  return statement;
}

DataBase::DataBase()
{
}

DataBase::~DataBase()
{
  // TODO Auto-generated destructor stub
}

DataBase& DataBase::operator=(const DataBase& other)
{
  _DB = other._DB;
  return *this;
}

int DataBase::open_database(const std::string& filename)
{
  int status = sqlite3_open(filename.c_str(), &_DB);
  if (status != SQLITE_OK)
  {
    std::cerr << "Error: Failed to open database " << filename << ":" << sqlite3_errmsg(_DB) << std::endl;
    if (!_DB)
      std::cerr << "Maybe an out of memory issue." << std::endl;
    return status;
  }
  return status;
}

void DataBase::close_database()
{
  int status = sqlite3_close(_DB);
  if (status != SQLITE_OK)
    std::cerr << "Error: Failed to close database " << sqlite3_errmsg(_DB) << std::endl;
}

int DataBase::create_table_Books()
{
  std::string statement = std::string("create table if not exists \"Books\" ") + "(\"Author\" text not null," + "\"Title\" text not null," + "\"Media_Type\" text not null,"
                          + "\"Read_Year\" integer not null," + "\"Comment\"   text, " + "primary key(\"Author\",\"Title\",\"Read_Year\")" + ")";
  std::string errmsg = execute_sql_insert_or_create(statement);
  if (errmsg != "")
  {
    std::cerr << statement << std::endl;
    std::cerr << errmsg << std::endl;
    return -1;
  }
  return 0;
}

int DataBase::fill_db_from_file(const std::string& filename)
{
  std::cout << create_table_Books() << std::endl;
  std::ifstream ifs(filename);
  if (!ifs.is_open())
  {
    std::cerr << "Error: Failed to open " << filename << std::endl;
    return -1;
  }
  int read_year = 0;
  std::string s;
  while (std::getline(ifs, s))
  {
    // Remove windows/dos-like line feeds (carriage return)
    s = remove_cr(s);
    // Remove leading and trailing space.characters
    s = trim(s);
    // Replace TABS with one space
    s = replace_tab_with_space(s);
    // Empty line
    if (s.empty())
      continue;
    // Comment line
    if (s[0] == '-')
      continue;
    Book book("Unknown", "Unknown", "bok", 0, "");
    std::istringstream iss(s);
    std::string str;
    if (getline(iss, str, ','))
    {
      if (str[0] == '2')
      {
        read_year = stoi(str);
        continue;
      }
      else
        book.set_author(trim(str));
    }
    if (getline(iss, str, ','))
    {
      book.set_title(trim(str));
    }
    if (getline(iss, str, ','))
      book.set_media_type(trim(str));
    book.set_read_year(read_year);
    if (getline(iss, str, ','))
      book.set_comment(trim(str));
    std::string statement = book.create_sql_insert_string();
    std::string errmsg = execute_sql_insert_or_create(statement);
    if (errmsg != "")
    {
      std::cerr << statement << std::endl;
      std::cerr << errmsg << std::endl;
    }
  }
  ifs.close();
  return 0;
}

int DataBase::save_db_to_file(const std::string& filename)
{
  std::ofstream ofs(filename);
  if (!ofs.is_open())
  {
    std::cerr << "Error: Failed to open " << filename << std::endl;
    return -1;
  }
  std::string sql_select_statement = "select * from Books order by Read_Year";
  std::vector<std::vector<std::string>> result;
  std::string errmsg = execute_sql_select(sql_select_statement, result);
  if (!errmsg.empty())
  {
    std::cerr << sql_select_statement << std::endl;
    std::cerr << errmsg << std::endl;
  }
  std::string current_year = iso_8859_1_to_utf8("0");
  ofs << iso_8859_1_to_utf8("-- Böcker som jag minns att jag blivit läst för som liten,"s) << std::endl;
  ofs << iso_8859_1_to_utf8("eller läst som ung:"s) << std::endl << std::endl;
  for (std::vector<std::string> vs : result)
  {
    // Read_Year is in position 3 of the string vector
    if (vs[3] != current_year)
    {
      current_year = iso_8859_1_to_utf8(vs[3]);
      if (vs[3] == iso_8859_1_to_utf8("2018"s) || vs[3] == iso_8859_1_to_utf8("2020"s))
      {
        ofs << std::endl << iso_8859_1_to_utf8("-- Lucka --") << std::endl;
      }
      if (vs[3] == iso_8859_1_to_utf8("2022"s))
      {
        ofs << iso_8859_1_to_utf8("-- Klinikboken --") << std::endl;
      }
      ofs << std::endl << current_year << std::endl << iso_8859_1_to_utf8("----"s) << std::endl;
    }
    bool first_col = true;
    for (std::string s : vs)
    {
      if (regexp_match(s, "^[0-9]+$"s) && s == current_year)
      {
        // skip the Read_Year column
        continue;
      }
      if (!first_col && s != "")
        ofs << iso_8859_1_to_utf8(", "s);
      else
        first_col = false;
      ofs << s;
    }
    ofs << std::endl;
  }
  ofs.close();
  return 0;
}

std::string DataBase::create_sql_select_statement(const std::string& title, const std::string& author, bool read_year_on, const std::string& read_year)
{
  // Transform to UTF-8 everything except title and author,
  // which are already in UTF-8 encoding when they come from
  // the gtk-GUI.
  std::string sql = iso_8859_1_to_utf8("select * from Books where Title like \"%"s) + title + iso_8859_1_to_utf8("%\" and Author like \"%"s) + author + iso_8859_1_to_utf8("%\""s);
  if (read_year_on)
    sql.append(iso_8859_1_to_utf8(" and Read_Year = \""s) + read_year + iso_8859_1_to_utf8("\""s));
  return sql;
}

std::string DataBase::execute_sql_insert_or_create(const std::string& statement)
{
  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(_DB, statement.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return sqlite3_errmsg(_DB);
  }
  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
  {
  }
  sqlite3_finalize(stmt);
  if (rc != SQLITE_DONE)
  {
    return sqlite3_errmsg(_DB);
  }
  return "";
}

std::string DataBase::execute_sql_select(const std::string& statement, std::vector<std::vector<std::string>>& result)
{
  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(_DB, statement.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return sqlite3_errmsg(_DB);
  }

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    std::vector<std::string> row;
    int n_cols = sqlite3_column_count(stmt);
    for (int i = 0; i < n_cols; i++)
    {
      const unsigned char* name = sqlite3_column_text(stmt, i);
      row.push_back((char*) name);
    }
    result.push_back(row);
  }
  sqlite3_finalize(stmt);
  if (rc != SQLITE_DONE)
  {
    return sqlite3_errmsg(_DB);
  }
  return "";
}

}/* end namespace MyBooks */

