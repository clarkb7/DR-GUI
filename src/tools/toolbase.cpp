/**************************************************************************
**
** 
** File: toolbase.cpp
** 
** Provides a structure through which tools will interface with the 
** mainwindow
** Anything needed by most or all tools should be provided here.
**
**
**************************************************************************/

#include <QtWidgets>

#include "toolbase.h"

ToolBase::ToolBase() {
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
}
ToolBase::ToolBase(QWidget *parent)
    :QWidget(parent) 
{
    ToolBase();
}

QString ToolBase::userFriendlyCurrentFile() {
    return strippedName(curFile);
}

void ToolBase::closeEvent(QCloseEvent *event) {
    if (true) {
        event->accept();
    } else {
        event->ignore();
    }
}

void ToolBase::setCurrentFile(const QString &fileName) {
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString ToolBase::strippedName(const QString &fullFileName) {
    return QFileInfo(fullFileName).fileName();
}