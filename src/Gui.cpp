/*
 * Gui.cpp
 *
 *  Created on: 14 sep. 2020
 *      Author: torsten
 */

#include "Gui.hpp"

namespace MyBooks
{

Gui::Gui(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::Window(cobject), _builder(builder), _title(""), _author(""), _read_year_on(false)
{

  // connect signals to callbacks
  Gtk::MenuItem *pExit = nullptr;
  _builder->get_widget("exit", pExit);
  if (pExit)
  {
    pExit->signal_activate().connect(sigc::mem_fun(*this, &Gui::on_exit_activated));
  }

  Gtk::MenuItem *pSaveDBToFile = nullptr;
  _builder->get_widget("savetofilemenuitem", pSaveDBToFile);
  if (pSaveDBToFile)
  {
    pSaveDBToFile->signal_activate().connect(sigc::mem_fun(*this, &Gui::on_savetofilemenuitem_activated));
  }

  Gtk::Button *pButton;
  _builder->get_widget("searchbutton", pButton);
  if (pButton)
  {
    pButton->signal_clicked().connect(sigc::mem_fun(*this, &Gui::on_searchbutton_clicked));
  }

  Gtk::Entry *pEntry;
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

  Gtk::CheckButton *pCheckButton;
  _builder->get_widget("yearcheckbutton", pCheckButton);
  if (pCheckButton)
  {
    pCheckButton->signal_clicked().connect(sigc::mem_fun(*this, &Gui::on_yearcheckbutton_toggled));
  }

  Gtk::SpinButton *pSpinButton;
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
Gui::Gui(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder, DataBase &db, const string &logfile) :
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

void Gui::on_file_dialog_response(int response_id, Gtk::FileChooserDialog *dialog)
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
      cerr << "Unexpected button clicked." << endl;
      break;
    }
  }
  delete dialog;

}

string Gui::choose_database_dir()
{

  auto dialog = new Gtk::FileChooserDialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog->set_title("Save to file:");
  dialog->set_transient_for(*this);
  dialog->set_modal(true);
  dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &Gui::on_file_dialog_response), dialog));
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

void Gui::on_savetofilemenuitem_activated()
{
  auto dialog = new Gtk::FileChooserDialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog->set_title("Save to file:");
  dialog->set_transient_for(*this);
  //dialog->set_modal(true);
  dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &Gui::on_file_dialog_response), dialog));
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
  string statement = _DB.create_sql_select_statement(_title, _author, _read_year_on, _read_year);
  vector<vector<string>> result;
  string errmsg = _DB.execute_sql_select(statement, result);
  if (errmsg != "")
  {
    cerr << statement << endl;
    cerr << "Select failed: " << errmsg << endl;
  }
  _refTreeModel->clear();
  for (vector<string> &rowvalues : result)
  {
    Gtk::TreeStore::iterator iter = _refTreeModel->append();
    Gtk::TreeModel::Row row = *iter;
    row[_columns._author] = rowvalues[0];
    row[_columns._title] = rowvalues[1];
    row[_columns._media_type] = rowvalues[2];
    row[_columns._read_year] = rowvalues[3];
    row[_columns._comment] = rowvalues[4];
  }
  for (vector<string> vs : result)
  {
    bool first_col = true;
    for (string s : vs)
    {
      if (!first_col && s != "")
        _logofs << ", ";
      else
        first_col = false;
      _logofs << s;
    }
    _logofs << endl;
  }
}

void Gui::on_title_changed()
{

  Gtk::Entry *pEntry = nullptr;
  _builder->get_widget("title", pEntry);
  if (pEntry)
  {
    _title = pEntry->get_text();
  }
  else
    cerr << "get_widget() title failed" << endl;
}

void Gui::on_author_changed()
{
  Gtk::Entry *pEntry;
  _builder->get_widget("author", pEntry);
  if (pEntry)
  {
    _author = pEntry->get_text();
  }
  else
    cerr << "get_widget() author failed" << endl;
}

void Gui::on_yearcheckbutton_toggled()
{
  Gtk::CheckButton *pButton;
  _builder->get_widget("yearcheckbutton", pButton);
  if (pButton)
  {
    _read_year_on = pButton->get_active();
    if (_read_year_on)
    {
      Gtk::SpinButton *pButton2;
      _builder->get_widget("yearspinbutton", pButton2);
      pButton2->set_editable();
    }
  }
  else
    cerr << "get_widget() yearcheckbutton failed" << endl;
}

void Gui::on_yearspinbutton_change_value()
{
  Gtk::SpinButton *pButton;
  _builder->get_widget("yearspinbutton", pButton);
  if (pButton)
  {
    _read_year = pButton->get_text();
  }
  else
    cerr << "get_widget() yearspinbutton failed" << endl;
}

} /* namespace MyBooks */
