set(SOURCE_FILES
    DataBase.cpp
    Gui.cpp
    MyBooks.cpp
)

#include_directories(${CMAKE_SOURCE_DIR}/src)

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(find_package(GTK2 REQUIRED) )


# add the executable
add_executable(MyBooks MyBooks.cpp ${SOURCE_FILES})

add_library(BookLib ${SOURCE_FILES})

target_link_libraries(MyBooks PUBLIC
gtkmm-3.0
atkmm-1.6
gdkmm-3.0
giomm-2.4
gtk-3
gdk-3
atk-1.0
cairo-gobject
gio-2.0
pangomm-1.4
glibmm-2.4
cairomm-1.0
sigc-2.0
pangocairo-1.0
pango-1.0
harfbuzz
cairo
gdk_pixbuf-2.0
gobject-2.0
glib-2.0
sqlite3
harfbuzz
gtkmm-3.0
atkmm-1.6
gdkmm-3.0
giomm-2.4
pangomm-1.4
glibmm-2.4
gtk-3
gdk-3
pangocairo-1.0
pango-1.0
atk-1.0
cairo-gobject
gio-2.0
cairomm-1.0
cairo
sigc-2.0
gdk_pixbuf-2.0
gobject-2.0
glib-2.0
pthread)


# To have a look at the CMake variables:
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
