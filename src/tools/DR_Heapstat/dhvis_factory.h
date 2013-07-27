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

/* dhvis_factory.h
 * 
 * Defines the Dr. Heapstat factory
 */

#ifndef DHVIS_FACTORY_H
#define DHVIS_FACTORY_H

#include "tools/drgui_tool_interface.h"

class dhvis_tool_t;
class dhvis_options_page_t;
struct dhvis_options_t;

class dhvis_factory_t : public drgui_tool_interface_t 
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DrGUI_ToolInterface_iid FILE "dhvis_metadata.json")
    Q_INTERFACES(drgui_tool_interface_t)

public:
    dhvis_factory_t(void);

    ~dhvis_factory_t(void);

    QStringList 
    tool_names(void) const;

    QWidget *
    create_instance(void);

    drgui_options_interface_t *
    create_options_page(void);

    void
    open_file(const QString &path, int line_num);

public slots:
    void
    update_settings(void);

private:
    /* GUI */
    QVector<dhvis_tool_t *> tool_instances;

    /* Options */
    dhvis_options_page_t *options_page;
    dhvis_options_t *options;
};

#endif