TEMPLATE      = lib
CONFIG       += plugin
QT           += widgets
QMAKE_CXXFLAGS += -g -Wall
INCLUDEPATH  += ../../
HEADERS         = ../drgui_tool_interface.h \
                  ../drgui_options_interface.h \
                  code_editor_structures.h \
                  highlighter.h \
                  code_editor.h \
                  code_editor_win.h \
                  code_editor_options_page.h \
                  code_editor_factory.h
SOURCES         = highlighter.cpp \
                  code_editor.cpp \
                  code_editor_win.cpp \
                  code_editor_options_page.cpp \
                  code_editor_factory.cpp
TARGET        = $$qtLibraryTarget(dg_code_editor)
DESTDIR       = ./tools