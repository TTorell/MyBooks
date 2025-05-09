/*
 * Gui.cpp
 *
 *  Created on: 14 sep. 2020
 *      Author: torsten
 */

#include "Gui.hpp"
#include <chrono>

using namespace std::string_literals;

namespace
{

std::string get_current_year() {
  using namespace std::chrono;
  return std::to_string(static_cast<int>(
      year_month_day{time_point_cast<days>(system_clock::now())}.year()));
}

auto internal_MyBooks_tektfile = "/home/torsten/eclipse-workspace/MyBooks/src/MyBooks.txt"s;

}

namespace MyBooks
{

// The first two parameters are mandatory in a constructor that will be called
// from Gtk::Builder::get_widget_derived().
// Additional parameters, if any, correspond to additional arguments in the call
// to Gtk::Builder::get_widget_derived().
Gui::Gui(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, DataBase& db, const std::string& logfile) :
    Gtk::Window(cobject), _builder(builder), _new_book_dialog(nullptr), _title(""), _author(""), _read_year_on(false), _DB(db)
{
  builder->get_widget_derived("newbookdialog", _new_book_dialog, _DB);
  _logofs.open(logfile);
  Gtk::SpinButton* p = nullptr;
  set_editable(p, _builder, "yearspinbutton", false);

  // connect signals to callbacks
  Gtk::MenuItem* p1 = nullptr;
  Gtk::Button* p2 = nullptr;
  Gtk::Entry* p3 = nullptr;
  Gtk::SpinButton* p4 = nullptr;
  //Gtk::TextView* p5 = nullptr;
  signal_activate_connect(p1, _builder, *this, "newbookmenuitem", &Gui::on_newbookmenuitem_activated);
  signal_activate_connect(p1, _builder, *this, "exit", &Gui::on_exit_activated);
  signal_activate_connect(p1, _builder, *this, "savetofilemenuitem", &Gui::on_savetofilemenuitem_activated);
  signal_clicked_connect(p2, _builder, *this, "searchbutton", &Gui::on_searchbutton_clicked);
  signal_changed_connect(p3, _builder, *this, "title", &Gui::on_title_changed);
  signal_changed_connect(p3, _builder, *this, "author", &Gui::on_author_changed);
  signal_clicked_connect(p2, _builder, *this, "yearcheckbutton", &Gui::on_yearcheckbutton_toggled);
  signal_changed_connect(p4, _builder, *this, "yearspinbutton", &Gui::on_yearspinbutton_change_value);
//  builder->get_widget("treeview", p5);
//  if (p5)
//  {
//    p5->signal_selection_get().connect(sigc::mem_fun(*this, &Gui::on_select_cursor_row));
//  }
//  else
//    std::cerr << "get_widget() " << "treeview" << " failed" << std::endl;

  //  signal_selected_connect(p5, _builder, *this, "treeview", &Gui::on_select_cursor_row);

  // initiate TreeView and its model
  _refTreeModel = Gtk::TreeStore::create(_columns);
  _builder->get_widget("treeview", _pTreeView);
  _pTreeView->set_model(_refTreeModel);

  // Append columns to treeview
  _pTreeView->append_column("Author", _columns._author);
  _pTreeView->append_column("Title", _columns._title);
  _pTreeView->append_column("Media type", _columns._media_type);
  _pTreeView->append_column("Read_year", _columns._read_year);
  _pTreeView->append_column("Comment", _columns._comment);

  //Make all the columns reorderable:
  //This is not necessary, but it's nice to show the feature.
  //You can use TreeView::set_column_drag_function() to more
  //finely control column drag and drop.
  for (unsigned int i = 0; i < _pTreeView->get_n_columns(); i++)
  {
    auto column = _pTreeView->get_column(static_cast<int>(i));
    column->set_reorderable();
  }
}

Gui::~Gui()
{
  _logofs.close();
}

void Gui::on_exit_activated()
{
  hide(); //hide() will cause Gtk::Application::run() to end.
}

void Gui::on_filedialog_response(int response_id, Gtk::FileChooserDialog* dialog)
{
  //Handle the response:
  switch (response_id)
  {
    case Gtk::RESPONSE_OK:
    {
      //Notice that this is a std::string, not a Glib::ustring.
      auto filename = dialog->get_file()->get_path();
      _DB.save_db_to_file(filename);
      break;
    }
    case Gtk::RESPONSE_CANCEL:
    {
      break;
    }
    default:
    {
      std::cerr << "Unexpected button clicked." << std::endl;
      break;
    }
  }
  delete dialog;
}

std::string Gui::choose_database_dir()
{

  auto dialog = new Gtk::FileChooserDialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog->set_title("Save to file:");
  dialog->set_transient_for(*this);
  dialog->set_modal(true);
  dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &Gui::on_filedialog_response), dialog));
  dialog->set_current_folder("/home/torsten/SQlite");
  dialog->set_current_name("MyBooks.txt");
  //Add response buttons to the dialog:
  dialog->add_button("cancel", Gtk::RESPONSE_CANCEL);
  dialog->add_button("save", Gtk::RESPONSE_OK);

  //Add filters, so that only certain file types can be selected:

  //    auto filter_text = Gtk::FileFilter::create();
  //    filter_text->set_name("Text files");
  //    filter_text->add_mime_type("text/plain");
  //    dialog->add_filter(filter_text);
  //
  //    auto filter_cpp = Gtk::FileFilter::create();
  //    filter_cpp->set_name("C/C++ files");
  //    filter_cpp->add_mime_type("text/x-c");
  //    filter_cpp->add_mime_type("text/x-c++");
  //    filter_cpp->add_mime_type("text/x-c-header");
  //    dialog->add_filter(filter_cpp);

  auto filter_any = Gtk::FileFilter::create();
  filter_any->set_name("Any files");
  filter_any->add_pattern("*");
  dialog->add_filter(filter_any);

  //Show the dialog and wait for a user response:
  dialog->show();
  return "";
}

void Gui::on_newbookmenuitem_activated()
{
  _new_book_dialog->reset();
  _new_book_dialog->show();
}

void Gui::on_savetofilemenuitem_activated()
{
  auto dialog = new Gtk::FileChooserDialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog->set_title("Save to file:");
  dialog->set_transient_for(*this);
  //dialog->set_modal(true);
  dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &Gui::on_filedialog_response), dialog));
  dialog->set_current_folder("/home/torsten/SQlite");
  dialog->set_current_name("MyBooks.txt");

  //Add response buttons to the dialog:
  dialog->add_button("cancel", Gtk::RESPONSE_CANCEL);
  dialog->add_button("save", Gtk::RESPONSE_OK);

//Add filters, so that only certain file types can be selected:

//    auto filter_text = Gtk::FileFilter::create();
//    filter_text->set_name("Text files");
//    filter_text->add_mime_type("text/plain");
//    dialog->add_filter(filter_text);
//
//    auto filter_cpp = Gtk::FileFilter::create();
//    filter_cpp->set_name("C/C++ files");
//    filter_cpp->add_mime_type("text/x-c");
//    filter_cpp->add_mime_type("text/x-c++");
//    filter_cpp->add_mime_type("text/x-c-header");
//    dialog->add_filter(filter_cpp);

  auto filter_any = Gtk::FileFilter::create();
  filter_any->set_name("Any files");
  filter_any->add_pattern("*");
  dialog->add_filter(filter_any);

  //Show the dialog and wait for a user response:
  dialog->show();
}

void Gui::on_searchbutton_clicked()
{
  std::string statement = _DB.create_sql_select_statement(_title, _author, _read_year_on, _read_year);
  statement.append(" order by Read_Year"s);
  std::vector<std::vector<std::string>> result;
  std::string errmsg = _DB.execute_sql_select(statement, result);
  if (errmsg != "")
  {
    std::cerr << statement << std::endl;
    std::cerr << "Select failed: " << errmsg << std::endl;
  }
  _refTreeModel->clear();
  for (std::vector<std::string>& rowvalues : result)
  {
    Gtk::TreeStore::iterator iter = _refTreeModel->append();
    Gtk::TreeModel::Row row = *iter;
    row[_columns._author] = rowvalues[0];
    row[_columns._title] = rowvalues[1];
    row[_columns._media_type] = rowvalues[2];
    row[_columns._read_year] = rowvalues[3];
    row[_columns._comment] = rowvalues[4];
  }
  for (std::vector<std::string> vs : result)
  {
    bool first_col = true;
    for (std::string s : vs)
    {
      if (!first_col && s != "")
        _logofs << ", ";
      else
        first_col = false;
      _logofs << s;
    }
    _logofs << std::endl;
  }
}

void Gui::on_title_changed()
{
  Gtk::Entry* p = nullptr;
  _title = get_text(p, _builder, "title");
}

void Gui::on_author_changed()
{
  Gtk::Entry* p = nullptr;
  _author = get_text(p, _builder, "author");
}

void Gui::on_yearcheckbutton_toggled()
{
  Gtk::CheckButton* p = nullptr;
  Gtk::SpinButton* p2 = nullptr;
  _read_year = get_text(p2, _builder, "yearspinbutton");
  _read_year_on = get_active(p, _builder, "yearcheckbutton");
  if (_read_year_on)
  {
    set_editable(p2, _builder, "yearspinbutton", true);
  }
  else
  {
    set_editable(p2, _builder, "yearspinbutton", false);
  }
}

void Gui::on_yearspinbutton_change_value()
{
  Gtk::SpinButton* p = nullptr;
  _read_year = get_text(p, _builder, "yearspinbutton");
}

void Gui::on_select_cursor_row()
{
  std::cout << "on_select_cursor_row" << std::endl;
}

NewBookDialog::NewBookDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& glade_builder, DataBase& db) :
    Gtk::Dialog(cobject), _builder(glade_builder), _DB(db)
{
  std::cout << "NewBookDialog()" << std::endl;
  Gtk::RadioButton* p1 = nullptr;
  Gtk::Entry* p2 = nullptr;
  Gtk::SpinButton* p3 = nullptr;
  Gtk::Button* p4 = nullptr;
  set_title("Insert a new book:");
  set_modal(true);
  signal_response().connect(sigc::bind(sigc::mem_fun(*this, &NewBookDialog::on_newbookdialog_response), this));
  // Connect signals to methods
  signal_toggled_connect(p1, _builder, *this, "bookradiobutton", &NewBookDialog::on_media_changed);
  signal_toggled_connect(p1, _builder, *this, "soundbookradiobutton", &NewBookDialog::on_media_changed);
  signal_toggled_connect(p1, _builder, *this, "magazineradiobutton", &NewBookDialog::on_media_changed);
  signal_toggled_connect(p1, _builder, *this, "dvdradiobutton", &NewBookDialog::on_media_changed);
  signal_toggled_connect(p1, _builder, *this, "otherradiobutton", &NewBookDialog::on_media_changed);
  signal_changed_connect(p2, _builder, *this, "title2", &NewBookDialog::on_title2_changed);
  signal_changed_connect(p2, _builder, *this, "author2", &NewBookDialog::on_author2_changed);
  signal_changed_connect(p3, _builder, *this, "yearspinbutton2", &NewBookDialog::on_yearspinbutton2_change_value);
  signal_changed_connect(p2, _builder, *this, "comment2", &NewBookDialog::on_comment2_changed);
  signal_clicked_connect(p4, _builder, *this, "cancelbutton2", &NewBookDialog::on_cancelbutton2_clicked);
  signal_clicked_connect(p4, _builder, *this, "savebutton2", &NewBookDialog::on_savebutton2_clicked);
}

NewBookDialog::~NewBookDialog()
{
}

void NewBookDialog::reset()
{
  std::cout << "reset" << std::endl;
  Gtk::Entry* p1 = nullptr;
  Gtk::SpinButton* p2 = nullptr;
  Gtk::RadioButton* p3 = nullptr;
  _book.set_title("");
  _book.set_author("");
  _book.set_media_type("Bok"s);
  _book.set_read_year(std::stoi(get_current_year()));
  _book.set_comment("");
  set_text(p1, _builder, "title2", "");
  set_text(p1, _builder, "author2", "");
  std::cout << "year: " << get_current_year() << std::endl;
  set_text(p2, _builder, "yearspinbutton2", get_current_year());
  std::cout << "year: " << get_text(p2, _builder, "yearspinbutton2") << std::endl;
  set_text(p1, _builder, "comment2", "");
  set_active(p3, _builder, "bookradiobutton", true);
  // GtkButton* savebutton = nullptr;
  // _builder->get_widget("savebutton2", savebutton);
  // gtk_widget_set_sensitive(GTK_WIDGET(savebutton), true);
}

void NewBookDialog::on_newbookdialog_response(int response_id, Gtk::Dialog* dialog)
{
//Handle the response:
  switch (response_id)
  {
    case Gtk::RESPONSE_OK:
    {
      std::cout << "RESPONSE_OK" << std::endl;
      std::cout << _book << std::endl;
      std::string error_msg = _DB.execute_sql_insert_or_create(_book.create_sql_insert_string());
      if (!error_msg.empty())
      {
        Gtk::Label* p = nullptr;
        set_text(p, _builder, "sqlerrorlabel", "");
        set_text(p, _builder, "sqlerrorlabel", error_msg);
        return;
      }
      _DB.save_db_to_file(internal_MyBooks_tektfile);
      break;
    }
    case Gtk::RESPONSE_CANCEL:
    {
      std::cout << "RESPONSE_CANCEL" << std::endl;
      break;
    }
    default:
    {
      std::cout << "DEFAULT_RESPONSE" << std::endl;
      std::cerr << "Unexpected button clicked." << std::endl;
      break;
    }
  }
  dialog->hide();
}

void NewBookDialog::on_title2_changed()
{
  std::cout << "on_title2_changed" << std::endl;
  Gtk::Entry* p = nullptr;
  _book.set_title(get_text(p, _builder, "title2"));

}

void NewBookDialog::on_savebutton2_clicked()
{
    std::cout << "on_savebutton2_clicked" << std::endl;
    std::cout << _book << std::endl;
    std::string error_msg =
        _DB.execute_sql_insert_or_create(_book.create_sql_insert_string());
    if (!error_msg.empty())
    {
        Gtk::Label *p = nullptr;
        set_text(p, _builder, "sqlerrorlabel", "");
        set_text(p, _builder, "sqlerrorlabel", error_msg);
        return;
    }
    _DB.save_db_to_file(internal_MyBooks_tektfile);
    hide();
}

void NewBookDialog::on_cancelbutton2_clicked()
{
  hide();
}

void NewBookDialog::check_specified_set_savebutton()
{
  if (_book.check_fully_specified())
  {
    GtkButton* savebutton2 = nullptr;
    _builder->get_object("savebutton2");
    gtk_widget_set_sensitive(GTK_WIDGET(savebutton2), true);
  }
}

void NewBookDialog::on_author2_changed()
{
  std::cout << "on_author2_changed" << std::endl;
  Gtk::Entry* p = nullptr;
  _book.set_author(get_text(p, _builder, "author2"));
  // GtkWidget* p1 = nullptr;
  // _builder->get_widget("savebutton2", p1);
  // if (_book.check_fully_specified())
  // {
  //   gtk_widget_set_sensitive(p1, true);
  // }
}

void NewBookDialog::on_yearspinbutton2_change_value()
{
  std::cout << "on_yearspinbutton2_change_value" << std::endl;
  Gtk::SpinButton* p = nullptr;
  std::string s = get_text(p, _builder, "yearspinbutton2");
  std::cout << "get_text: "s << s << std::endl;
  if (s.empty())
    _book.set_read_year(0);
  else
    _book.set_read_year(std::stoi(s));
}

void NewBookDialog::on_media_changed()
{
  std::cout << "on_media_changed" << std::endl;
  Gtk::RadioButton* p = nullptr;
  auto media = ""s;
  std::vector<std::pair<std::string, std::string>> widget_name_vector =
  { {"bookradiobutton", "Book"},
    {"soundbookradiobutton", "Soundbook"},
    {"magazineradiobutton", "Magazineb"},
    {"dvdradiobutton", "DVD"},
    {"otherradiobutton", "Other"}
  };
  for(auto widget_pair:widget_name_vector)
  {
    // Only one alternative can be active at a time.
    if (get_active(p, _builder, widget_pair.first))
    {
      media = widget_pair.second;
    }
  }
  _book.set_media_type(media);
}

void NewBookDialog::on_comment2_changed()
{
  std::cout << "on_comment2_changed" << std::endl;
  Gtk::Entry* p = nullptr;
  _book.set_comment(get_text(p, _builder, "comment2"));
}

} /* namespace MyBooks */
