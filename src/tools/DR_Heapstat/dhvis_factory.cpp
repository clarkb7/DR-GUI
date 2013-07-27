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

/* dhvis_factory.cpp
 * 
 * Provides the Dr. Heapstat factory
 */

#ifdef __CLASS__
#  undef __CLASS__
#endif
#define __CLASS__ "dhvis_factory_t::"

#include <QDebug>

#include "tools/drgui_tool_interface.h"
#include "tools/drgui_options_interface.h"
#include "dhvis_structures.h"
#include "dhvis_graph.h"
#include "dhvis_options_page.h"
#include "dhvis_tool.h"
#include "dhvis_factory.h"


/* Public
 * Constructor
 */
dhvis_factory_t::dhvis_factory_t(void) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    options_page = new dhvis_options_page_t;
    connect(options_page, SIGNAL(settings_changed()),
            this, SLOT(update_settings()));
    options = new dhvis_options_t;
    /* loads settings */
    create_options_page();
}

/* Public
 * Destructor
 */
dhvis_factory_t::~dhvis_factory_t(void) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    delete options_page;
    delete options;
    while (tool_instances.count() > 0) {
        dhvis_tool_t *tmp = tool_instances.back();
        tool_instances.pop_back();
        delete tmp;
    }
}

/* Public
 * Returns provided tool-names during loading
 */
QStringList 
dhvis_factory_t::tool_names(void) const 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    return QStringList() << "Dr. Heapstat";
}

/* Public
 * Returns a new instance of the tool
 */
QWidget *
dhvis_factory_t::create_instance(void) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    tool_instances.append(new dhvis_tool_t(options));
    connect(tool_instances.back(), SIGNAL(code_editor_requested(QFile &, int)),
            this, SIGNAL(code_editor_requested(QFile &, int)));
    return tool_instances.back();
}

/* Public
 * Refreshes and returns the options page
 */
drgui_options_interface_t *
dhvis_factory_t::create_options_page(void) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    options_page->set_options(options);
    return options_page;
}

/* Public slot
 * Tells each tab to update after settings change
 */
void
dhvis_factory_t::update_settings(void)
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    foreach (dhvis_tool_t *tool, tool_instances) {
        tool->update_settings();
    }
}

/* Public
 * Unused virtual implementation
 */
void
dhvis_factory_t::open_file(const QString &path, int line_num)
{
    Q_UNUSED(path);
    Q_UNUSED(line_num);
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
}