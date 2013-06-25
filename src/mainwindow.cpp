/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: mainwindow.cpp
** 
** Provides a main structure for users to interface with tools.
**
**************************************************************************/

#include <QtWidgets>
#include <QActionGroup>

#include "mainwindow.h"
#include "tools/toolbase.h"

/* Public
   Constructor, everything begins here
*/
MainWindow::MainWindow() {
    windowMapper = new QSignalMapper(this);
    tabArea = new QTabWidget;
    tabArea->setTabsClosable(true);
    tabArea->setMovable(true);
    connect(tabArea, SIGNAL(tabCloseRequested(int)),
            this, SLOT(maybeClose(int)));
    setCentralWidget(tabArea);
    
    connect(tabArea, SIGNAL(currentChanged(int)),
            this, SLOT(updateMenus()));

    createActions();
    createMenus();
    createStatusBar();
    updateMenus();
    loadTools();

    readSettings();

    setWindowTitle(tr("DR-GUI"));
    setUnifiedTitleAndToolBarOnMac(true);
}

/* Protected
   Handles closing of all tabs
*/
void MainWindow::closeEvent(QCloseEvent *event) {
    closeAllTabs();
    if (tabArea->currentWidget()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

/* Private Slot
   Shows about page for this program
*/
void MainWindow::about() {
   QMessageBox::about(this, tr("About DR-GUI"),
            tr("<center><b>DR-GUI</b></center><br>"
               "Interface for Dynamorio and various extensions"));
}

/* Slot
   Updates the menus to reflect current tab's abilities
*/
void MainWindow::updateMenus() {
    bool hasToolBase = (activeToolBase() != 0);
    closeAct->setEnabled(hasToolBase);
    closeAllAct->setEnabled(hasToolBase);
    nextAct->setEnabled(hasToolBase);
    previousAct->setEnabled(hasToolBase);
    separatorAct->setVisible(hasToolBase);
}

/* Private Slot
   Updates the Window menu to reflect current tab's abilities
*/
void MainWindow::updateWindowMenu() {
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    separatorAct->setVisible(tabArea->currentWidget());

    for (int index = 0; index < tabArea->count(); ++index) {
        ToolBase *tool = qobject_cast<ToolBase *>(tabArea->widget(index));

        QString text;
        if (index < 9) {
            text = tr("&%1 %2").arg(index + 1)
                               .arg(tabArea->tabText(index) + ": " +
                                    tool->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(index + 1)
                              .arg(tabArea->tabText(index) + ": " +
                                   tool->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(tool == activeToolBase());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, index);
        connect(windowMapper, SIGNAL(mapped(int)), 
                tabArea, SLOT(setCurrentIndex(int)));
    }
}

/* Private Slot
   Creates and connects the actions for the mainwindow
*/
void MainWindow::createActions() {
    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    /* File */
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), 
            qApp, SLOT(closeAllWindows()));

    /* Edit */
    preferencesAct = new QAction(tr("&Preferences"), this);
    preferencesAct->setStatusTip(tr("Edit Preferences"));
    connect(preferencesAct, SIGNAL(triggered()), 
            this, SLOT(showPreferencesDialog()));

    /* Window */
    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active tab"));
    connect(closeAct, SIGNAL(triggered()),
            this, SLOT(maybeCloseMe()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the tabs"));
    connect(closeAllAct, SIGNAL(triggered()),
            this, SLOT(closeAllTabs()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next tab"));
    connect(nextAct, SIGNAL(triggered()),
            this, SLOT(activateNextTab()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "tab"));
    connect(previousAct, SIGNAL(triggered()),
            this, SLOT(activatePreviousTab()));

    /* Help */
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), 
            this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), 
            qApp, SLOT(aboutQt()));

    /* Tools */
    toolActionGroup = new QActionGroup(this);
}

/* Private
   Creates the menus in the menu bar of the mainwindow
*/
void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addSeparator();
    QAction *action = fileMenu->addAction(tr("Switch layout direction"));
    connect(action, SIGNAL(triggered()), 
            this, SLOT(switchLayoutDirection()));
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(preferencesAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    toolMenu = menuBar()->addMenu(tr("&Tool"));
}

/* Private
   Creates status bar for displaying status tips
*/
void MainWindow::createStatusBar() {
    statusBar()->showMessage(tr("Ready"));
}

/* Private
   loads ini settings for mainwindow
*/
void MainWindow::readSettings() {
    QSettings settings("Dynamorio", "DR-GUI");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

/* Private
   Saves ini settings for mainwindow
*/
void MainWindow::writeSettings() {
    QSettings settings("Dynamorio", "DR-GUI");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

/* Private Slot
   Switches direction of layout for mainwindow
*/
void MainWindow::switchLayoutDirection() {
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

/* Private
   finds and returns active ToolBase tab, if there is one
*/
ToolBase *MainWindow::activeToolBase() {
    int activeTab = tabArea->currentIndex();
    if ( activeTab != -1)
        return qobject_cast<ToolBase *>(tabArea->currentWidget());
    return 0;
}

/* Private Slot
   Displays preferences dialog
*/
void MainWindow::showPreferencesDialog() {
    //OptionBase optBase;
    //optBase.exec();
}

/* Private Slot
   Closes every tab in mainwindow
*/
void MainWindow::closeAllTabs() {
    int max = tabArea->count();
    for (int tabCount = 0; tabCount < max; ++tabCount) {
        tabArea->removeTab(0);
    }
}

/* Private Slot
   helper for closing current tab
*/
void MainWindow::maybeCloseMe() {
    maybeClose(tabArea->currentIndex());
}

/* Private Slot
   Confirms closing of a tab
*/
void MainWindow::maybeClose(int index) {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Confirm"),
                    tr("Are you sure you want to close '%1'?")
                        .arg(tabArea->tabText(index)),
                    QMessageBox::Yes | QMessageBox::No |
                    QMessageBox::Cancel);
    if (ret == QMessageBox::Yes)
        hideTab(index);
}

/* Private Slot
   Closes a tab
*/
void MainWindow::hideTab(int index) {
    tabArea->removeTab(index);
}

/* Private Slot
   Moves view to next tab in order, loops
*/
void MainWindow::activateNextTab() {
    int index = tabArea->currentIndex() + 1;
    if(index == tabArea->count())
        index = 0;
    tabArea->setCurrentIndex(index);
}

/* Private Slot
   Movies view to previous tab in order, loops
*/
void MainWindow::activatePreviousTab() {
    int index = tabArea->currentIndex() - 1;
    if(index == -1)
        index = tabArea->count() - 1;
    tabArea->setCurrentIndex(index);
}

/* Private
   Loads available tools
*/
void MainWindow::loadTools()
{
    pluginsDir = QDir(qApp->applicationDirPath());

    #if defined(Q_OS_WIN)
        /*  TODO */
    #elif defined(Q_OS_MAC)
        /* TODO */
    #endif
    pluginsDir.cd("tools");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            ToolInterface *itool = qobject_cast<ToolInterface *>(plugin);
            if (itool)
                addToMenu(plugin, itool->toolNames(), 
                          toolMenu, SLOT(addTab()), toolActionGroup);
            pluginFileNames += fileName;
        }
    }
}

/* Private
   Adds a tool to toolsMenu
*/
void MainWindow::addToMenu(QObject *plugin, const QStringList &texts,
                           QMenu *menu, const char *member,
                           QActionGroup *actionGroup)
{
    foreach (QString text, texts) {
        QAction *action = new QAction(text, plugin);
        connect(action, SIGNAL(triggered()), this, member);
        menu->addAction(action);

        if (actionGroup) {
            action->setCheckable(true);
            actionGroup->addAction(action);
        }
    }
}
/* Private Slot
   Creates a new instance of a tool
   and displays it in the tab interface
*/
void MainWindow::addTab() {
    QAction *action = qobject_cast<QAction *>(sender());
    ToolBase *tool = qobject_cast<ToolBase *>
                       (qobject_cast<ToolInterface *>
                         (action->parent())->createInstance());
    const QString toolName = action->text();

    tabArea->addTab(tool, toolName);
}

