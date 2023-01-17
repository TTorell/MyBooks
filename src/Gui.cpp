/*
 * Gui.cpp
 *
 *  Created on: 14 sep. 2020
 *      Author: torsten
 */

#include "Gui.hpp"
#include <chrono>
namespace
{

std::string get_current_year()
{
  const auto now = std::chrono::system_clock::now();
  auto hours = static_cast<double>(std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch()).count());
  return std::to_string(static_cast<int>(1970 + (hours / (24.0 * 365.0)))); //approximately
}

}

namespace MyBooks
{

Gui::Gui(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
    Gtk::Window(cobject), _builder(builder), _new_book_dialog(_builder), _title(""), _author(""), _read_year_on(false)
{

  // connect signals to callbacks

  Gtk::MenuItem* pNewBook = nullptr;
  _builder->get_widget("newbookmenuitem", pNewBook);
  if (pNewBook)
  {
    pNewBook->signal_activate().connect(sigc::mem_fun(*this, &Gui::on_newbookmenuitem_activated));
  }

  Gtk::MenuItem* pExit = nullptr;
  _builder->get_widget("exit", pExit);
  if (pExit)
  {
    pExit->signal_activate().connect(sigc::mem_fun(*this, &Gui::on_exit_activated));
  }

  Gtk::MenuItem* pSaveDBToFile = nullptr;
  _builder->get_widget("savetofilemenuitem", pSaveDBToFile);
  if (pSaveDBToFile)
  {
    pSaveDBToFile->signal_activate().connect(sigc::mem_fun(*this, &Gui::on_savetofilemenuitem_activated));
  }

  Gtk::Button* pButton;
  _builder->get_widget("searchbutton", pButton);
  if (pButton)
  {
    pButton->signal_clicked().connect(sigc::mem_fun(*this, &Gui::on_searchbutton_clicked));
  }

  Gtk::Entry* pEntry;
  _builder->get_widget("title", pEntry);
  if (pEntry)
  {
    pEntry->signal_changed().connect(sigc::mem_fun(*this, &Gui::on_title_changed));
  }

  _builder->get_widget("author", pEntry);
  if (pEntry)
  {
    pEntry->signal_changed().connect(sigc::mem_fun(*this, &Gui::on_author_changed));
  }

  Gtk::CheckButton* pCheckButton;
  _builder->get_widget("yearcheckbutton", pCheckButton);
  if (pCheckButton)
  {
    pCheckButton->signal_clicked().connect(sigc::mem_fun(*this, &Gui::on_yearcheckbutton_toggled));
  }

  Gtk::SpinButton* pSpinButton;
  _builder->get_widget("yearspinbutton", pSpinButton);
  if (pSpinButton)
  {
    _read_year = pSpinButton->get_text();
    pSpinButton->signal_changed().connect(sigc::mem_fun(*this, &Gui::on_yearspinbutton_change_value));
  }
  // initiate TreView and its model
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
    auto column = _pTreeView->get_column(i);
    column->set_reorderable();
  }
}

// The first two parameters are mandatory in a constructor that will be called
// from Gtk::Builder::get_widget_derived().
// Additional parameters, if any, correspond to additional arguments in the call
// to Gtk::Builder::get_widget_derived().
Gui::Gui(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, DataBase& db, const std::string& logfile) :
    Gui(cobject, builder)
{
  _DB = db;
  _logofs.open(logfile);
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
  _new_book_dialog.reset();
  _new_book_dialog.show();
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
  Gtk::CheckButton* pButton;
  _builder->get_widget("yearcheckbutton", pButton);
  if (pButton)
  {
    _read_year_on = pButton->get_active();
    if (_read_year_on)
    {
      Gtk::SpinButton* pButton2;
      _builder->get_widget("yearspinbutton", pButton2);
      pButton2->set_editable();
    }
  }
  else
    std::cerr << "get_widget() yearcheckbutton failed" << std::endl;
}

void Gui::on_yearspinbutton_change_value()
{
  Gtk::SpinButton* p = nullptr;
  _read_year = get_text(p, _builder, "yearspinbutton");
}

NewBookDialog::NewBookDialog(Glib::RefPtr<Gtk::Builder> builder) :
    _builder(builder)
{
  _builder->get_widget("newbookdialog", _dialog);
  _dialog->set_title("Insert a new book:");
  _dialog->set_modal(true);
  _dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &NewBookDialog::on_newbookdialog_response), _dialog));
  //Add response buttons to the dialog:
  _dialog->add_button("cancel", Gtk::RESPONSE_CANCEL);
  _dialog->add_button("save", Gtk::RESPONSE_OK);

  Gtk::RadioButton* pButton;
  _builder->get_widget("bookradiobutton", pButton);
  if (pButton)
  {
    pButton->signal_toggled().connect(sigc::mem_fun(*this, &NewBookDialog::on_bookradiobutton_toggled));
  }
  _builder->get_widget("soundbookradiobutton", pButton);
  if (pButton)
  {
    pButton->signal_toggled().connect(sigc::mem_fun(*this, &NewBookDialog::on_soundbookradiobutton_toggled));
  }

  Gtk::Entry* pEntry;
  _builder->get_widget("title2", pEntry);
  if (pEntry)
  {
    pEntry->signal_changed().connect(sigc::mem_fun(*this, &NewBookDialog::on_title2_changed));
  }

  _builder->get_widget("author2", pEntry);
  if (pEntry)
  {
    pEntry->signal_changed().connect(sigc::mem_fun(*this, &NewBookDialog::on_author2_changed));
  }

  Gtk::SpinButton* pSpinButton;
  _builder->get_widget("yearspinbutton2", pSpinButton);
  if (pSpinButton)
  {
    _read_year = pSpinButton->get_text();
    pSpinButton->signal_changed().connect(sigc::mem_fun(*this, &NewBookDialog::on_yearspinbutton2_change_value));
  }

  _builder->get_widget("comment2", pEntry);
  if (pEntry)
  {
    pEntry->signal_changed().connect(sigc::mem_fun(*this, &NewBookDialog::on_comment2_changed));
  }
}

NewBookDialog::~NewBookDialog()
{
  delete _dialog;
  _dialog = nullptr;
}

void NewBookDialog::show()
{
  _dialog->show();
}

void NewBookDialog::reset()
{
  std::cout << "reset" << std::endl;
  Gtk::Entry* p1 = nullptr;
  Gtk::SpinButton* p2 = nullptr;
  Gtk::RadioButton* p3 = nullptr;
  _book.set_title("");
  _book.set_author("");
  _book.set_media_type("Media_type::Book");
  _book.set_read_year(std::stoi(get_current_year()));
  _book.set_comment("");
  set_text(p1, _builder, "title2", "");
  set_text(p1, _builder, "author2", "");
  std::cout << "year: " << get_current_year() << std::endl;
  set_text(p2, _builder, "yearspinbutton2", get_current_year());
  set_text(p1, _builder, "comment2", "");
  set_active(p3, _builder, "bookradiobutton", true);
  set_active(p3, _builder, "soundbookradiobutton", false);
}

void NewBookDialog::on_newbookdialog_response(int response_id, Gtk::Dialog* dialog)
{
  //Handle the response:
  switch (response_id)
  {
    case Gtk::RESPONSE_OK:
    {
//      //Notice that this is a std::string, not a Glib::ustring.
//      auto filename = dialog->get_file()->get_path();
//      _DB.save_db_to_file(filename);
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
  dialog->hide();
}

void NewBookDialog::on_title2_changed()
{
  std::cout << "on_title2_changed" << std::endl;
  Gtk::Entry* p = nullptr;
  _book.set_title(get_text(p, _builder, "title2"));
}

void NewBookDialog::on_author2_changed()
{
  std::cout << "on_author2_changed" << std::endl;
  Gtk::Entry* p = nullptr;
  _book.set_author(get_text(p, _builder, "author2"));
}

void NewBookDialog::on_yearspinbutton2_change_value()
{
  std::cout << "on_yearspinbutton2_change_value" << std::endl;
  Gtk::SpinButton* p = nullptr;
  _book.set_read_year((std::stoi(get_text(p, _builder, "yearspinbutton2"))));
}


void NewBookDialog::on_bookradiobutton_toggled()
{
  std::cout << "on_bookradiobutton_toggled" << std::endl;
  Gtk::RadioButton* p = nullptr;
  _book.set_media_type(get_active(p, _builder, "bookradiobutton") ? "Media_type::Book" : "Media_type::Soundbook");
}

void NewBookDialog::on_soundbookradiobutton_toggled()
{
  std::cout << "on_soundbookradiobutton_toggled" << std::endl;
  Gtk::RadioButton* p = nullptr;
  _book.set_media_type(get_active(p, _builder, "soundbookradiobutton") ? "Media_type::Soundbook" : "Media_type::Book");
}

void NewBookDialog::on_comment2_changed()
{
  std::cout << "on_comment2_changed" << std::endl;
  Gtk::Entry* p = nullptr;
  _book.set_comment(get_text(p, _builder, "comment2"));
}


} /* namespace MyBooks */
