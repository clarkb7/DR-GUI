TEMPLATE      = lib
CONFIG       += plugin
QT           += widgets
QMAKE_CXXFLAGS += -g
INCLUDEPATH  += ../../
HEADERS       = ../toolbase.h \
                ../options_interface.h \
                ./dr_heap_structures.h \
                ./dr_heap_options.h \
                ./dr_heap_tool.h \
                ./dr_heap_factory.h \
                ./dr_heap_graph.h
SOURCES       = ../toolbase.cpp \
                ./dr_heap_options.cpp \
                ./dr_heap_tool.cpp \
                ./dr_heap_factory.cpp \
                ./dr_heap_graph.cpp 
TARGET        = $$qtLibraryTarget(dg_DR_Heapstat)
DESTDIR       = ./tools