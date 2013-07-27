/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: code_editor_factory.cpp
** 
** Provides the code_editor factory
**
*************************************************************************/

#include <QDebug>

#include "tools/drgui_tool_interface.h"
#include "tools/drgui_options_interface.h"
#include "code_editor_structures.h"
#include "code_editor_options_page.h"
#include "code_editor_win.h"
#include "code_editor_factory.h"


/* Public
 * Constructor
 */
code_editor_factory_t::code_editor_factory_t(void) 
{
    qDebug() << "INFO: Entering code_editor_factory_t::code_editor_factory_t"
                "(void)";
    options_page = new code_editor_options_page_t;
    connect(options_page, SIGNAL(settings_changed()),
            this, SLOT(update_settings()));
    options = new options_t;
    /* loads settings */
    create_options_page();
}

/* Public
 * Destructor
 */
code_editor_factory_t::~code_editor_factory_t(void) 
{
    qDebug() << "INFO: Entering code_editor_factory_t::~code_editor_factory_t"
                "(void)";
    delete options_page;
    delete options;
    for(int i = 0; i < tool_instances.count();) {
        code_editor_win_t *tmp = tool_instances.back();
        tool_instances.pop_back();
        delete tmp;
    }
}

/* Public
 * Returns provided tool-names during loading
 */
QStringList 
code_editor_factory_t::tool_names(void) const 
{
    qDebug() << "INFO: Entering code_editor_factory_t::tool_names(void)";
    return QStringList() << "Code Editor";
}

/* Public
 * Returns a new instance of the tool
 */
QWidget *
code_editor_factory_t::create_instance(void) 
{
    qDebug() << "INFO: Entering code_editor_factory_t::create_instance(void)";
    tool_instances.append(new code_editor_win_t(options));
    return tool_instances.back();
}

/* Public
 * Refreshes and returns the options page
 */
drgui_options_interface_t *
code_editor_factory_t::create_options_page(void) 
{
    qDebug() << "INFO: Entering code_editor_factory_t::create_options_page"
                "(void)";
    options_page->set_options(options);
    return options_page;
}

/* Public slot
 * tells each tab to update after settings change
 */
void
code_editor_factory_t::update_settings(void)
{
    qDebug() << "INFO: Entering code_editor_factory_t::update_settings(void)";
    foreach (code_editor_win_t *tool, tool_instances) {
        tool->update_settings();
    }
}

/* Public
 * opens file in code_editor
 * only called right after creating the window
 */
void
code_editor_factory_t::open_file(const QString &path, int line_num)
{
    tool_instances.back()->open_file(path, line_num);
}