/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: dr_heap_factory.cpp
** 
** Provides the DR. Heapstat factory
**
*************************************************************************/

#include <QDebug>

#include "tools/toolbase.h"
#include "tools/options_interface.h"
#include "dr_heap_structures.h"
#include "dr_heap_graph.h"
#include "dr_heap_options.h"
#include "dr_heap_tool.h"
#include "dr_heap_factory.h"


/* Public
 * Constructor
 */
dr_heapstat_factory_t::dr_heapstat_factory_t(void) 
{
    qDebug() << "INFO: Entering dr_heapstat_factory_t::dr_heapstat_factory_t"
                "(void)";
    options_page = new dr_heapstat_options_t;
    connect(options_page, SIGNAL(settings_changed()),
            this, SLOT(update_settings()));
    options = new options_t;
    /* loads settings */
    create_options_page();
}

/* Public
 * Destructor
 */
dr_heapstat_factory_t::~dr_heapstat_factory_t(void) 
{
    qDebug() << "INFO: Entering dr_heapstat_factory_t::~dr_heapstat_factory_t"
                "(void)";
    delete options_page;
    delete options;
    for(int i = 0; i < tool_instances.count();) {
        dr_heapstat_t *tmp = tool_instances.back();
        tool_instances.pop_back();
        delete tmp;
    }
}

/* Public
 * Returns provided tool-names during loading
 */
QStringList 
dr_heapstat_factory_t::tool_names(void) const 
{
    qDebug() << "INFO: Entering dr_heapstat_factory_t::tool_names(void)";
    return QStringList() << "Dr. Heapstat";
}

/* Public
 * Returns a new instance of the tool
 */
QWidget *
dr_heapstat_factory_t::create_instance(void) 
{
    qDebug() << "INFO: Entering dr_heapstat_factory_t::create_instance(void)";
    tool_instances.append(new dr_heapstat_t(options));
    return tool_instances.back();
}

/* Public
 * Refreshes and returns the options page
 */
options_interface_t *
dr_heapstat_factory_t::create_options_page(void) 
{
    qDebug() << "INFO: Entering dr_heapstat_factory_t::create_options_page"
                "(void)";
    options_page->set_options(options);
    return options_page;
}

/* Public slot
 * tells each tab to update after settings change
 */
void
dr_heapstat_factory_t::update_settings(void)
{
    qDebug() << "INFO: Entering dr_heapstat_factory_t::update_settings(void)";
    foreach (dr_heapstat_t *tool, tool_instances) {
        tool->update_settings();
    }
}