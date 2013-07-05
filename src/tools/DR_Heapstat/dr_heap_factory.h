/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: dr_heap_factory.h
** 
** Defines the DR. Heapstat factory
**
*************************************************************************/

#ifndef DR_HEAP_FACTORY_H
#define DR_HEAP_FACTORY_H

#include "tools/toolbase.h"

class dr_heapstat_t;
class dr_heapstat_options_t;
struct options_t;

class dr_heapstat_factory_t : public tool_interface_t 
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ToolInterface_iid FILE "DR_Heapstat.json")
    Q_INTERFACES(tool_interface_t)

public:
    dr_heapstat_factory_t(void);

    ~dr_heapstat_factory_t(void);

    QStringList 
    tool_names(void) const;

    QWidget *
    create_instance(void);

    options_interface_t *
    create_options_page(void);

public slots:
    void
    update_settings(void);

private:
    /* GUI */
    QVector<dr_heapstat_t *> tool_instances;

    /* Options */
    dr_heapstat_options_t *options_page;
    options_t *options;
};

#endif