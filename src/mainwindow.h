/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: mainwindow.h
** 
** Defines a main structure for users to interface with tools.
**
**************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QStringList>

class OptionBase;
class ToolBase;
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QActionGroup;
class QPluginLoader;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void about();
    void updateMenus();
    void updateWindowMenu();
    void switchLayoutDirection();
    void showPreferencesDialog();
    void maybeCloseMe();
    void maybeClose(int index);
    void addTab();
    void hideTab(int index);
    void closeAllTabs();
    void activateNextTab();
    void activatePreviousTab();
    
private:
    void createActions();
    void createMenus();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    ToolBase *activeToolBase();
    void loadTools();
    void addToMenu(QObject *plugin, const QStringList &texts,
                           QMenu *menu, const char *member,
                           QActionGroup *actionGroup);

    /* GUI */
    QDir pluginsDir;
    QStringList pluginFileNames;
    QTabWidget *tabArea;
    QSignalMapper *windowMapper;
    QActionGroup *toolActionGroup;
    QAction *separatorAct;

    QMenu *fileMenu;
    /*Switch layout act */
    QAction *exitAct;

    QMenu *editMenu;
    QAction *preferencesAct;

    QMenu *windowMenu;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *nextAct;
    QAction *previousAct;

    QMenu *helpMenu;    
    QAction *aboutAct;
    QAction *aboutQtAct;

    QMenu *toolMenu;
};

#endif
