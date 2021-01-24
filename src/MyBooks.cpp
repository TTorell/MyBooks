/*
 * Books.cpp
 *
 *  Created on: 5 juni 2020
 *      Author: torsten
 */

#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <string>
#include <vector>
#include <glib.h>
#include <gtkmm.h>
#include <filesystem>

#include "Gui.hpp"
#include "DataBase.hpp"
extern "C" {
#include <time.h>
}
namespace fs = std::filesystem;
using namespace std;

using namespace MyBooks;

int main(int argc, char *argv[])
{

  clock_t start, stop;
  string books_textfile = "/home/torsten/Documents/Ljudböcker/Böcker_och_ljudböcker.txt";
  string logfile = "/home/torsten/Documents/Ljudböcker/MyBooks_log.txt";
  string gladefile = "/home/torsten/eclipse-workspace/MyBooks/src/Books.glade";
  error_code error;

  start = clock();
  // Create an SQlite directory in users home directory if it doesn't exist.
  // We'll put the database file there if we must create a new DB.
  char *val = getenv("HOME");
  if (val == NULL)
  {
    cerr << "Couldn't read environment variable HOME." << endl;
    return -1;
  }
  string home = val;

  fs::path db_dir(home + "/SQlite");
  if (!fs::exists(db_dir))
    if (!fs::create_directories(db_dir, error))
      cerr << "Couldn't create database directory: " << db_dir << " " << error.message() << endl;

  // Create new, or open existing, database "MyBooks.db" in "$HOME/SQlite/".
  DataBase db;
  int status = db.open_database(db_dir / "MyBooks.db");
  if (status)
  {
    cerr << "Couldn't open database " << endl;
    return -1;
  }

  stop = clock();
  cout << (stop - start) << " us" << endl;
  //cout << CLOCKS_PER_SEC << endl;

  // Fill DB from textfile.
  db.fill_db_from_file(books_textfile);

  // Create gui.
  auto app = Gtk::Application::create(argc, argv, "org.basic.glade");

  //Load the Glade file and instantiate its widgets.

  // Another way of doing it:
  // Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("/home/torsten/eclipse-workspace/MyBooks/src/Books.glade");

  auto builder = Gtk::Builder::create();
  try
  {
    builder->add_from_file(gladefile);
  }
  catch (const Glib::FileError &ex)
  {
    cerr << "FileError: " << ex.what() << std::endl;
    return 1;
  }
  catch (const Glib::MarkupError &ex)
  {
    cerr << "MarkupError: " << ex.what() << std::endl;
    return 1;
  }
  catch (const Gtk::BuilderError &ex)
  {
    cerr << "BuilderError: " << ex.what() << std::endl;
    return 1;
  }

  Gui *gui = NULL;
  builder->get_widget_derived("mainwindow", gui, db, logfile);

  if (gui)
  {
    // cout << gui->choose_database_dir() << endl;

    app->run(*gui, argc, argv);
    // or just app->run(*gui);
  }
  db.close_database();

  return 0;
}
;
