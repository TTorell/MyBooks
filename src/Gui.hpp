/*
 * Gui.hpp
 *
 *  Created on: 14 sep. 2020
 *      Author: torsten
 */

#ifndef GUI_HPP_
#define GUI_HPP_

#include <gtkmm.h>
#include <iostream>
#include <string>
#include "DataBase.hpp"

using namespace std;
namespace MyBooks
{

class Gui: public Gtk::Window
{
  public:
    Gui(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
    Gui(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade, DataBase& DB, const string& logfile);
    virtual ~Gui();
    string choose_database_dir();

  protected:
    //Signal handlers:
    void on_exit_activated();
    void on_file_dialog_response(int response_id, Gtk::FileChooserDialog* dialog);
    void on_savetofilemenuitem_activated();
    void on_searchbutton_clicked();
    void on_title_changed();
    void on_author_changed();
    void on_yearspinbutton_changed();
    void on_yearcheckbutton_toggled();
    void on_yearspinbutton_change_value();

    //member variables:
    ofstream _logofs;
    Glib::RefPtr<Gtk::Builder> _builder;
    string _title;
    string _author;
    bool _read_year_on;
    string _read_year;
    DataBase _DB;

    //Inner class: Tree model columns for TreeStore:
    class ModelColumns: public Gtk::TreeModel::ColumnRecord
    {
      public:

        ModelColumns()
        {
          add(_title);
          add(_author);
          add(_media_type);
          add(_read_year);
          add(_comment);
        }

        Gtk::TreeModelColumn<string> _title;
        Gtk::TreeModelColumn<string> _author;
        Gtk::TreeModelColumn<string> _media_type;
        Gtk::TreeModelColumn<string> _read_year;
        Gtk::TreeModelColumn<string> _comment;
    };
    ModelColumns _columns;

    Glib::RefPtr<Gtk::TreeStore> _refTreeModel;
    Gtk::TreeView* _pTreeView;
};


} /* namespace MyBooks */

#endif
