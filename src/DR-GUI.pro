QT += widgets

HEADERS       = mainwindow.h \
                tools/toolbase.h \
                
SOURCES       = main.cpp \
                mainwindow.cpp \
                \
                tools/toolbase.cpp \
                
LIBS          = -L./tools

# install
target.path = ./DR-GUI
INSTALLS += target

simulator: warning(This example might not fully work on Simulator platform)