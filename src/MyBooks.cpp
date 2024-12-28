/*
 * MyBooks.cpp
 *
 *  Created on: 5 june 2020
 *      Author: torsten
 *
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
#if defined (__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#endif
#include <gtkmm.h>
#if defined (__clang__)
#pragma clang diagnostic pop
#endif
#include <filesystem>

#include "Gui.hpp"
#include "DataBase.hpp"
extern "C"
{
#include <time.h>
}
namespace fs = std::filesystem;


int main(int argc, char* argv[])
{

  clock_t start, stop;
  std::string books_textfile = "/home/torsten/Documents/Ljudböcker/Böcker_och_ljudböcker.txt";
  std::string logfile = "/home/torsten/Documents/Ljudböcker/MyBooks_log.txt";
  std::string gladefile = "/home/torsten/eclipse-workspace/MyBooks/src/Books.glade";
  std::error_code error;

  start = clock();
  // Create an SQlite directory in users home directory if it doesn't exist.
  // We'll put the database file there if we must create a new DB.
  char* val = getenv("HOME");
  if (val == nullptr)
  {
    std::cerr << "Couldn't read environment variable HOME." << std::endl;
    return -1;
  }
  std::string home = val;

  fs::path db_dir(home + "/SQlite");
  if (!fs::exists(db_dir))
    if (!fs::create_directories(db_dir, error))
      std::cerr << "Couldn't create database directory: " << db_dir << " " << error.message() << std::endl;

  // Create new, or open existing, database "MyBooks.db" in "$HOME/SQlite/".
  MyBooks::DataBase db;
  int status = db.open_database(db_dir / "MyBooks.db");
  if (status)
  {
    std::cerr << "Couldn't open database " << std::endl;
    return -1;
  }

  stop = clock();
  std::cout << (stop - start) << " us" << std::endl;
  //std::cout << CLOCKS_PER_SEC << std::endl;

  // Fill DB from text file.
  //db.fill_db_from_file(books_textfile);

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
  catch (const Glib::FileError& ex)
  {
    std::cerr << "FileError: " << ex.what() << std::endl;
    return 1;
  }
  catch (const Glib::MarkupError& ex)
  {
    std::cerr << "MarkupError: " << ex.what() << std::endl;
    return 1;
  }
  catch (const Gtk::BuilderError& ex)
  {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
    return 1;
  }

  MyBooks::Gui* gui = NULL;
  builder->get_widget_derived("mainwindow", gui, db, logfile);

  if (gui)
  {
    // std::cout << gui->choose_database_dir() << std::endl;

    app->run(*gui, argc, argv);
    // or just app->run(*gui);
  }
  db.close_database();

  return 0;
};
