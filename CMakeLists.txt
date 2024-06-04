cmake_minimum_required(VERSION 3.20)

set( CMAKE_VERBOSE_MAKEFILE TRUE )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


cmake_policy(SET CMP0145 NEW)

# set the project name
project(MyBooks LANGUAGES CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(CTest)

configure_file(MyBooksConfig.h.in MyBooksConfig.h)

# set output directories for all builds (Debug, Release, etc.)
#foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
#    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
#    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib ) # lib.a
#    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib ) # lib.so
#    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin ) # binaries
#endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib ) # lib.a
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib ) # lib.so
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin ) # binaries

# This list was taken from the command-output of "pkg-config gtkmm-3.0 --cflags".
# There is probably a way of using pkg-config directly in CMake.

include_directories("/home/torsten/eclipse-workspace/MyBooks/src"
                    "/usr/include/gtkmm-3.0"
                    "/usr/lib/x86_64-linux-gnu/gtkmm-3.0/include"
                    "/usr/include/atkmm-1.6"
                    "/usr/lib/x86_64-linux-gnu/atkmm-1.6/include "
                    "/usr/include/gtk-3.0/unix-print"
                    "/usr/include/gdkmm-3.0"
                    "/usr/lib/x86_64-linux-gnu/gdkmm-3.0/include"
                    "/usr/include/giomm-2.4"
                    "/usr/lib/x86_64-linux-gnu/giomm-2.4/include"
                    "/usr/include/gtk-3.0"
                    "/usr/include/at-spi2-atk/2.0"
                    "/usr/include/at-spi-2.0"
                    "/usr/include/dbus-1.0"
                    "/usr/lib/x86_64-linux-gnu/dbus-1.0/include"
                    "/usr/include/gtk-3.0"
                    "/usr/include/gio-unix-2.0"
                    "/usr/include/cairo"
                    "/usr/include/atk-1.0"
                    "/usr/include/cairo"
                    "/usr/include/pangomm-1.4"
                    "/usr/lib/x86_64-linux-gnu/pangomm-1.4/include"
                    "/usr/include/glibmm-2.4"
                    "/usr/lib/x86_64-linux-gnu/glibmm-2.4/include"
                    "/usr/include/cairomm-1.0"
                    "/usr/lib/x86_64-linux-gnu/cairomm-1.0/include"
                    "/usr/include/sigc++-2.0"
                    "/usr/lib/x86_64-linux-gnu/sigc++-2.0/include"
                    "/usr/include/pango-1.0"
                    "/usr/include/harfbuzz"
                    "/usr/include/pango-1.0"
                    "/usr/include/fribidi"
                    "/usr/include/harfbuzz"
                    "/usr/include/cairo"
                    "/usr/include/pixman-1"
                    "/usr/include/uuid"
                    "/usr/include/freetype2"
                    "/usr/include/gdk-pixbuf-2.0"
                    "/usr/include/libpng16"
                    "/usr/include/x86_64-linux-gnu"
                    "/usr/include/libmount"
                    "/usr/include/blkid"
                    "/usr/include/glib-2.0"
                    "/usr/lib/x86_64-linux-gnu/glib-2.0/include")

# specify compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /W4")
    # Default debug flags are OK in windows
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
    # Tried /favour:{INTEL64} as well to make it faster on my own system,
    # but the build system didn't seem to understand, and It's hardly
    # applicable to everyone anyway.

else()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g0")

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES /usr/include/c++/13 /usr/include/x86_64-linux-gnu/c++/13 /usr/include/c++/13/backward /usr/lib/llvm-18/lib/clang/18/include /usr/local/include /usr/include/x86_64-linux-gnu /usr/include)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -m64 -march=native -msse2 -pedantic -pedantic-errors -Wall -Wextra -Werror -Wcast-qual -Wshadow -Wconversion -Wdouble-promotion -fmessage-length=0 -stdlib=libstdc++ -pthread")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++ -lc++abi")
    else()
        # for gcc at the moment
        set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES /usr/include/c++/13 /usr/include/x86_64-linux-gnu/c++/13 /usr/include/c++/13/backward /usr/lib/gcc/x86_64-linux-gnu/13/include /usr/local/include /usr/include/x86_64-linux-gnu /usr/include)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -m64 -march=native -msse2 -pedantic -pedantic-errors -Wall -Wextra -Werror -Wcast-qual -Wshadow -Wconversion -Wdouble-promotion -fmessage-length=0 -pthread")
    endif()
endif()


if(UNIX AND NOT APPLE)
    # for Linux, BSD, Solaris, Minix

    # target_link_libraries(C2-chessengine pthread)
    # is  defined with -pthread compiler-flag instead

    # Don't know if the --stack flag has any effect on linkers in LINUX.
    # In Ubuntu it seems like you get approximately 8 megabytes as default.
    # You can change stacksize in the bash-shell from cmd-line with "ulimit -s <size>"
    # Tried, in vain, the following just to test anyhow:
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,10000") didn't work
    # or set_target_properties(<target> PROPERTIES LINK_FLAGS -Wl,--stack,10000") didn't work
else()
    # In windows, I think we only get 1 Mb of stack default,
    # so I'm trying to specify 10 Mb:
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:10000")
endif()

enable_testing()

add_subdirectory(src)
#add_subdirectory(tests)

# To have a look at the CMake variables:
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
