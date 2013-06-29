/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: toolbase.cpp
** 
** Provides the structures through which tools will interface with the 
** mainwindow
** Anything needed by most or all tools should be provided here.
**
**************************************************************************/

#include <QtWidgets>

#include "toolbase.h"

/* Public
   Constructor,
*/
tool_base_t::tool_base_t() 
{
    setAttribute(Qt::WA_DeleteOnClose);
    is_untitled = true;
}

/* Public
   Constructor,
*/
tool_base_t::tool_base_t(QWidget *parent)
    :QWidget(parent) 
{
    tool_base_t();
}

/* Public
   Provides just filename and extension
*/
QString 
tool_base_t::user_friendly_current_file() 
{
    return stripped_name(cur_file);
}

/* Protected
   Close or cancel?
*/
void 
tool_base_t::closeEvent(QCloseEvent *event) 
{
    if (true) {
        event->accept();
    } else {
        event->ignore();
    }
}

/* Private
   Sets the current file and adjusts display accordingly
*/
void 
tool_base_t::set_current_file(const QString &file_name) 
{
    cur_file = QFileInfo(file_name).canonicalFilePath();
    is_untitled = false;
    setWindowModified(false);
    setWindowTitle(user_friendly_current_file() + "[*]");
}

/* Private
   Provides just filename and extension
*/
QString 
tool_base_t::stripped_name(const QString &full_file_name) 
{
    return QFileInfo(full_file_name).fileName();
}
