/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: toolbase.h
** 
** Defines the structures through which tools will interface with the
** mainwindow
** Anything needed by most or all tools should be defined here.
**
**************************************************************************/

#ifndef TOOL_BASE_H
#define TOOL_BASE_H

#include <QWidget>
#include <QtPlugin>

class QMenu;

class tool_base_t : public QWidget
{
    Q_OBJECT

public:
    tool_base_t(void);

    tool_base_t(QWidget *parent);

    QString 
    user_friendly_current_file();
    
    QString 
    current_file() { return cur_file; }

protected:
    void 
    closeEvent(QCloseEvent *event);

private:
    void 
    set_current_file(const QString &file_name);
    
    QString 
    stripped_name(const QString &full_file_name);

    QString cur_file;
    bool is_untitled;

};

#endif

#ifndef TOOL_INTERFACE_H
#define TOOL_INTERFACE_H

class tool_interface_t 
{
public:
    virtual 
    QStringList tool_names() const = 0;
    
    virtual 
    QObject *create_instance() = 0;

};

#define ToolInterface_iid "org.DR-GUI.ToolInterface"

Q_DECLARE_INTERFACE(tool_interface_t, ToolInterface_iid)

#endif
