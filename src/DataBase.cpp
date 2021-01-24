/*
 * DataBase.cpp
 *
 *  Created on: 16 sep. 2020
 *      Author: torsten
 */

#include "DataBase.hpp"
#include <cstring>
#include <regex>

// File-private functions
namespace
{

// My database is UTF-8 encoded.
// So we need a convert-function
string iso_8859_1_to_utf8(string &str)
{
  string str_out;
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

string iso_8859_1_to_utf8(const char *c_string)
{
  string str_out(c_string);
  return iso_8859_1_to_utf8(str_out);
}

// match a regular expression in a string
bool regexp_match(const string &s, const string &regexp_string)
{
  smatch m;
  regex r(regexp_string);
  return (regex_match(s, m, r));
}

// replace TAB-characters with one space-character
string replace_tab_with_space(const string &s)
{
  return regex_replace(s, regex("\\t"), string(" "));
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
string ltrim(const string &s)
{
  return regex_replace(s, regex("^\\s+"), string(""));
}

// Remove trailing space characters from string
string rtrim(const string &s)
{
  return regex_replace(s, regex("\\s+$"), string(""));
}

string trim(const string &s)
{
  return ltrim(rtrim(s));
}

// Remove windows/dos-like line-feeds (carriage return)
string remove_cr(const string &s)
{
  return regex_replace(s, regex("\\r$"), string(""));
}
} // End namespace

namespace MyBooks
{

Book::Book(const string &title, const string &author, const string &media_type, int read_year, const string &comment) :
    _author(author), _title(title), _media_type(media_type), _read_year(read_year), _comment(comment)
{
}

string Book::create_sql_insert_string()
{
  string statement = "INSERT INTO Books (Author, Title, Media_Type, Read_Year,Comment) VALUES (\"" + _author + "\", \"" + _title + "\",\"" + _media_type + "\",\""
                     + to_string(_read_year) + "\",\"" + _comment + "\")";
  // My database uses UFT-8 character encoding so;
  return iso_8859_1_to_utf8(statement);
}

DataBase::DataBase()
{
}

DataBase::~DataBase()
{
  // TODO Auto-generated destructor stub
}

DataBase& DataBase::operator=(const DataBase &other)
{
  _DB = other._DB;
  return *this;
}

int DataBase::open_database(const string &filename)
{
  int status = sqlite3_open(filename.c_str(), &_DB);
  if (status != SQLITE_OK)
  {
    cerr << "Error: Failed to open database " << filename << ":" << sqlite3_errmsg(_DB) << endl;
    if (!_DB)
      cerr << "Maybe an out of memory issue." << endl;
    return status;
  }
  return status;
}

void DataBase::close_database()
{
  int status = sqlite3_close(_DB);
  if (status != SQLITE_OK)
    cerr << "Error: Failed to close database " << sqlite3_errmsg(_DB) << endl;
}

int DataBase::create_table_Books()
{
  string statement = string("create table if not exists \"Books\" ") + "(\"Author\" text not null," + "\"Title\" text not null," + "\"Media_Type\" text not null,"
                     + "\"Read_Year\" integer not null," + "\"Comment\"   text, " + "primary key(\"Author\",\"Title\",\"Read_Year\")" + ")";
  string errmsg = execute_sql_insert_or_create(statement);
  if (errmsg != "")
  {
    cerr << statement << endl;
    cerr << errmsg << endl;
    return -1;
  }
  return 0;
}

int DataBase::fill_db_from_file(const string &filename)
{
  cout << create_table_Books() << endl;
  ifstream ifs(filename);
  if (!ifs.is_open())
  {
    cerr << "Error: Failed to open " << filename << endl;
    return -1;
  }
  int read_year = 0;
  string s;
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
    istringstream iss(s);
    string str;
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
    string statement = book.create_sql_insert_string();
    string errmsg = execute_sql_insert_or_create(statement);
    if (errmsg != "")
    {
      cerr << statement << endl;
      cerr << errmsg << endl;
    }
  }
  ifs.close();
  return 0;
}

int DataBase::save_db_to_file(const string &filename)
{
  ofstream ofs(filename);
  if (!ofs.is_open())
  {
    cerr << "Error: Failed to open " << filename << endl;
    return -1;
  }
  string sql_select_statement = "select * from Books order by Read_Year";
  vector<vector<string>> result;
  string errmsg = execute_sql_select(sql_select_statement, result);
  if (!errmsg.empty())
  {
    cerr << sql_select_statement << endl;
    cerr << errmsg << endl;
  }
  string current_year = "0";
  ofs << iso_8859_1_to_utf8("-- Böcker som jag minns att jag läst eller blivit läst för som liten:") << endl << endl;
  for (vector<string> vs : result)
  {
    // Read_Year is in position 3 of the string vector
    if (vs[3] != current_year)
    {
      current_year = vs[3];
      ofs << endl << current_year << endl << "----" << endl;
    }
    bool first_col = true;
    for (string s : vs)
    {
      if (regexp_match(s, "^[0-9]+$") && s == current_year)
      {
        // skip the Read_Year column
        continue;
      }
      if (!first_col && s != "")
        ofs << ", ";
      else
        first_col = false;
      ofs << s;
    }
    ofs << endl;
  }
  ofs.close();
  return 0;
}

string DataBase::create_sql_select_statement(const string &title, const string &author, bool read_year_on, const string &read_year)
{
  // Transform to UTF-8 everything except title and author,
  // which are already in UTF-8 encoding when they come from
  // the gtk-GUI.
  string sql = iso_8859_1_to_utf8("select * from Books where Title like \"%") + title + iso_8859_1_to_utf8("%\" and Author like \"%") + author + iso_8859_1_to_utf8("%\"");
  if (read_year_on)
    sql.append(iso_8859_1_to_utf8(" and Read_Year = \"") + read_year + iso_8859_1_to_utf8("\""));
  return sql;

}

string DataBase::execute_sql_insert_or_create(const string &statement)
{
  sqlite3_stmt *stmt;
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

string DataBase::execute_sql_select(const string &statement, vector<vector<string>> &result)
{
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(_DB, statement.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return sqlite3_errmsg(_DB);
  }

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    vector<string> row;
    int n_cols = sqlite3_column_count(stmt);
    for (int i = 0; i < n_cols; i++)
    {
      const unsigned char *name = sqlite3_column_text(stmt, i);
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

