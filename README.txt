This is just a Linux C++ program I wrote to learn GUI-programming with GTK+3.0,
Glade GUI-design-tool and an SQlite3 database. Actually, it's a mix between
Glade and the Gtkmm C++-classes which I find more appealing than doing it
in plain C.

It keeps track of the books I've read and listened to as soundbooks.
(Why not, some kind of data has to be stored in the database.)
it provides some search possibilities on Author, Title and the year
when I read It. The database can be populated from a comma-separated
textfile, MyBooks.txt (in swedish). I had some problems with the swedish
native character set, but solved it. I also got tricked by some
Book-titles which themselves contained commas.
It's not complete (No adding of new books yet, for instance, must do that
in the textfile.)

It contains a GUI-class and a DB-class which someone could take
"code-snippets" from.

If anyone ever cares to make it build in their environment:
First you must install glade, gtkmm3.0, gtk+3.0, sqlite3, and maybe some more,
on your system. It runs on linux, but requieres a lot of include-paths, libs
etc. It's probably easiest to use the pkg-config command to find reuired paths,
flags and libs. Not sure if it is complete, but something like:
> pkg-config --cflags --libs gtk+-3.0 gtkmm-3.0 sqlite3
Maybe I'll provide a cmake file to build it with, later.
For now I use eclipse on Ubuntu.
I think it requires c++17 (or later) because of some "std::filesystem::path
thing" I used. Yoy'll have to change some file paths in MyBooks.cpp too.

I chose a Gnu LGPL (Lesser) license just beacuse Gtk, Glade and Gtkmm are
resealsed under it. SQLite3 is in the public domain and does not require a
license.

