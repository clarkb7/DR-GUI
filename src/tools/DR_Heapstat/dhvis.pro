TEMPLATE      = lib
CONFIG       += plugin
QT           += widgets
QMAKE_CXXFLAGS += -g -Wall
INCLUDEPATH  += ../../
HEADERS       = ../drgui_tool_interface.h \
                ../drgui_options_interface.h \
                ./dhvis_structures.h \
                ./dhvis_options_page.h \
                ./dhvis_tool.h \
                ./dhvis_factory.h \
                ./dhvis_graph.h \
                ./dhvis_stale_graph.h
SOURCES       = ./dhvis_options_page.cpp \
                ./dhvis_tool.cpp \
                ./dhvis_factory.cpp \
                ./dhvis_graph.cpp \
                ./dhvis_stale_graph.cpp 
TARGET        = $$qtLibraryTarget(dg_dhvis)
DESTDIR       = ./tools