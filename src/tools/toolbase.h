/**************************************************************************
**
** 
** File: toolbase.h
** 
** Defines a structure through which tools will interface with the mainwindow
** Anything needed by most or all tools should be defined here.
**
**
**************************************************************************/

#ifndef TOOLBASE_H
#define TOOLBASE_H

#include <QWidget>
#include <QtPlugin>

class QMenu;


class ToolBase : public QWidget
{
    Q_OBJECT

public:
    ToolBase();
    ToolBase(QWidget *parent);

    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

protected:
    void closeEvent(QCloseEvent *event);

private slots:

private:
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    bool isUntitled;
};
#endif

#ifndef TOOLINTERFACE_H
#define TOOLINTERFACE_H

class ToolInterface {
public:
    virtual QStringList toolNames() const = 0;
    virtual QObject* createInstance() = 0;
};

#define ToolInterface_iid "org.DR-GUI.ToolInterface"

Q_DECLARE_INTERFACE(ToolInterface, ToolInterface_iid)
#endif
