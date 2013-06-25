TEMPLATE      = lib
CONFIG       += plugin
DEFINES      += QT_NO_DEBUG_OUTPUT
QT           += widgets
INCLUDEPATH  += ../../
HEADERS       = ../toolbase.h \
                ./dr_heap_tool.h \
                ./dr_heap_graph.h
SOURCES       = ../toolbase.cpp \
                ./dr_heap_tool.cpp \
                ./dr_heap_graph.cpp
TARGET        = $$qtLibraryTarget(dg_DR_Heapstat)
DESTDIR       = ./tools