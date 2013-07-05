/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: options_window.h
** 
** Provides a main interface for users to adjust options for tools
**
*************************************************************************/

#ifndef OPTIONS_WINDOW_H
#define OPTIONS_WINDOW_H

#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QHBoxLayout;
class QVBoxLayout;
class QActionGroup;

class options_window_t : public QDialog
{
    Q_OBJECT

public:
    options_window_t(QActionGroup *tool_action_group_);

    void
    display(void);

public slots:
    void 
    change_page(QListWidgetItem *current, QListWidgetItem *previous);

private slots:
    void
    save(void);

private:
    void
    create_tool_list(void);
    
    void
    read_settings(void);

    /* GUI */
    QVBoxLayout *main_layout;
    QHBoxLayout *horizontal_layout;
    QListWidget *tool_page_list;
    QStackedWidget *tool_page_stack;

    QHBoxLayout *buttons_layout;
    QPushButton *close_button;
    QPushButton *save_button;

    QActionGroup *tool_action_group;


};

#endif