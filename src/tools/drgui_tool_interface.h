/* **********************************************************
 * Copyright (c) 2013, Branden Clark All rights reserved.
 * **********************************************************/

/* Dr. Heapstat Visualizer
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the conditions outlined in
 * the BSD 2-Clause license are met.
 
 * This software is provided by the copyright holders and contributors "AS IS"
 * and any express or implied warranties, including, but not limited to, the
 * implied warranties of merchantability and fitness for a particular purpose
 * are disclaimed. See the BSD 2-Clause license for more details.
 */

/* drgui_tool_interface.h
 * 
 * Defines the structures through which tools will interface with the
 * mainwindow
 * Anything needed by most or all tools should be defined here.
 */

#include <QWidget>
#include <QtPlugin>
#include <QFile>

#ifndef DRGUI_TOOL_INTERFACE_H
#define DRGUI_TOOL_INTERFACE_H

#include "drgui_options_interface.h"

class drgui_tool_interface_t : public QWidget
{
    Q_OBJECT
public:
    virtual 
    QStringList 
    tool_names() const = 0;
    
    virtual 
    QWidget *
    create_instance(void) = 0;

    virtual
    drgui_options_interface_t *
    create_options_page(void) = 0;

    virtual
    void
    open_file(const QString &path, int line_num) = 0;

signals:
    void
    code_editor_requested(QFile &file, int line_num);

};

#define DrGUI_ToolInterface_iid "org.DR-GUI.ToolInterface"

Q_DECLARE_INTERFACE(drgui_tool_interface_t, DrGUI_ToolInterface_iid)

#endif
