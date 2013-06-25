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
ToolBase::ToolBase() {
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
}

/* Public
   Constructor,
*/
ToolBase::ToolBase(QWidget *parent)
    :QWidget(parent) {
    ToolBase();
}

/* Public
   Provides just filename and extension
*/
QString ToolBase::userFriendlyCurrentFile() {
    return strippedName(curFile);
}

/* Protected
   Close or cancel?
*/
void ToolBase::closeEvent(QCloseEvent *event) {
    if (true) {
        event->accept();
    } else {
        event->ignore();
    }
}

/* Private
   Sets the current file and adjusts display accordingly
*/
void ToolBase::setCurrentFile(const QString &fileName) {
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

/* Private
   Provides just filename and extension
*/
QString ToolBase::strippedName(const QString &fullFileName) {
    return QFileInfo(fullFileName).fileName();
}