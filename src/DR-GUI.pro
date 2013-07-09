QT += widgets
QMAKE_CXXFLAGS += -g
HEADERS       = mainwindow.h \
                tools/toolbase.h \
                options/options_window.h
                
SOURCES       = main.cpp \
                mainwindow.cpp \
                \
                tools/toolbase.cpp \
                options/options_window.cpp
                
LIBS          = -L./tools

# install
target.path = ./DR-GUI
INSTALLS += target

simulator: warning(This example might not fully work on Simulator platform)