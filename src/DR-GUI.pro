QT += widgets
QMAKE_CXXFLAGS += -g -Wall
HEADERS       = tools/drgui_tool_interface.h \
                drgui_main_window.h \
                options/drgui_options_window.h
                
SOURCES       = main.cpp \
                drgui_main_window.cpp \
                options/drgui_options_window.cpp
                
LIBS          = -L./tools

# install
target.path = ./DR-GUI
INSTALLS += target

simulator: warning(This example might not fully work on Simulator platform)