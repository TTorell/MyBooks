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

namespace MyBooks
{

template<typename T, typename T_base_object>
void signal_activate_connect(T* p, Glib::RefPtr<Gtk::Builder> builder, T_base_object& object, const std::string& widget_name, void (T_base_object::*func)())
{
  builder->get_widget(widget_name, p);
  if (p)
  {
    p->signal_activate().connect(sigc::mem_fun(object, func));
  }
  else
    std::cerr << "get_widget() " << widget_name << " failed" << std::endl;
}

template<typename T, typename T_base_object>
void signal_clicked_connect(T* p, Glib::RefPtr<Gtk::Builder> builder, T_base_object& object, const std::string& widget_name, void (T_base_object::*func)())
{
  builder->get_widget(widget_name, p);
  if (p)
  {
    p->signal_clicked().connect(sigc::mem_fun(object, func));
  }
  else
    std::cerr << "get_widget() " << widget_name << " failed" << std::endl;
}

template<typename T, typename T_base_object>
void signal_changed_connect(T* p, Glib::RefPtr<Gtk::Builder> builder, T_base_object& object, const std::string& widget_name, void (T_base_object::*func)())
{
  builder->get_widget(widget_name, p);
  if (p)
  {
    p->signal_changed().connect(sigc::mem_fun(object, func));
  }
  else
    std::cerr << "get_widget() " << widget_name << " failed" << std::endl;
}

template<typename T, typename T_base_object>
void signal_toggled_connect(T* p, Glib::RefPtr<Gtk::Builder> builder, T_base_object& object, const std::string& widget_name, void (T_base_object::*func)())
{
  builder->get_widget(widget_name, p);
  if (p)
  {
    p->signal_toggled().connect(sigc::mem_fun(object, func));
  }
  else
    std::cerr << "get_widget() " << widget_name << " failed" << std::endl;
}

template<typename T>
void set_text(T* p, Glib::RefPtr<Gtk::Builder> builder, const std::string& widget_name, const std::string& text)
{
  builder->get_widget(widget_name, p);
  if (p)
  {
    p->set_text(text);
  }
  else
    std::cerr << "get_widget() " << widget_name << " failed" << std::endl;
}

template<typename T>
std::string get_text(T* p, Glib::RefPtr<Gtk::Builder> builder, const std::string& widget_name)
{
  builder->get_widget(widget_name, p);
  if (p)
  {
    return p->get_text();
  }
  else
    std::cerr << "get_widget() " << widget_name << " failed" << std::endl;
  return "";
}

template<typename T>
void set_value(T* p, Glib::RefPtr<Gtk::Builder> builder, const std::string& widget_name, const double value)
{
  builder->get_widget(widget_name, p);
  if (p)
  {
    p->set_value(value);
  }
  else
    std::cerr << "get_widget() " << widget_name << " failed" << std::endl;
}

template<typename T>
bool get_active(T* p, Glib::RefPtr<Gtk::Builder> builder, const std::string& widget_name)
{
  builder->get_widget(widget_name, p);
  if (p)
  {
    return p->get_active();
  }
  else
    std::cerr << "get_widget() " << widget_name << " failed" << std::endl;
  return false;
}

template<typename T>
void set_active(T* p, Glib::RefPtr<Gtk::Builder> builder, const std::string& widget_name, const bool value)
{
  builder->get_widget(widget_name, p);
  if (p)
  {
    p->set_active(value);
  }
  else
    std::cerr << "get_widget() " << widget_name << " failed" << std::endl;
}

enum class Media_type
{
  Book,
  Soundbook
};

class NewBookDialog : public Gtk::Dialog
{
  private:
    Glib::RefPtr<Gtk::Builder> _builder;
    Book _book;
    DataBase& _DB;
  public:
    NewBookDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& glade_builder, DataBase& db);
    ~NewBookDialog();
    void reset();
    void on_title2_changed();
    void on_author2_changed();
    void on_yearspinbutton2_change_value();
    void on_comment2_changed();
    void on_bookradiobutton_toggled();
    void on_soundbookradiobutton_toggled();
    void on_newbookdialog_response(int response_id, Gtk::Dialog* dialog);
    void on_yearspinbutton_changed();
    void on_cancelbutton_clicked();
    void on_insertbutton_clicked();
};

class Gui: public Gtk::Window
{
  public:
    Gui(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade, DataBase& db, const std::string& logfile);
    virtual ~Gui();
    std::string choose_database_dir();

  protected:

    //member variables:
    std::ofstream _logofs;
    Glib::RefPtr<Gtk::Builder> _builder;
    NewBookDialog* _new_book_dialog = nullptr;
    std::string _title;
    std::string _author;
    bool _read_year_on;
    std::string _read_year;
    DataBase& _DB;

    //Signal handlers:
    void on_exit_activated();
    void on_filedialog_response(int response_id, Gtk::FileChooserDialog* dialog);
    void on_newbookmenuitem_activated();
    void on_savetofilemenuitem_activated();
    void on_searchbutton_clicked();
    void on_title_changed();
    void on_author_changed();
    void on_yearspinbutton_changed();
    void on_yearcheckbutton_toggled();
    void on_yearspinbutton_change_value();

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

        Gtk::TreeModelColumn<std::string> _title;
        Gtk::TreeModelColumn<std::string> _author;
        Gtk::TreeModelColumn<std::string> _media_type;
        Gtk::TreeModelColumn<std::string> _read_year;
        Gtk::TreeModelColumn<std::string> _comment;
    };

    ModelColumns _columns;

    Glib::RefPtr<Gtk::TreeStore> _refTreeModel;
    Gtk::TreeView* _pTreeView;
};

} /* namespace MyBooks */

#endif
