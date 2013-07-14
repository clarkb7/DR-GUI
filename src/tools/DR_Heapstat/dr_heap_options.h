/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: dr_heap_options.h
** 
** Defines the DR. Heapstat options page
**
*************************************************************************/

#ifndef DR_HEAPSTAT_OPTIONS_H
#define DR_HEAPSTAT_OPTIONS_H

#include "tools/options_interface.h"
#include "dr_heap_structures.h"

class QSpinBox;
class QLineEdit;
class QCheckBox;

class dr_heapstat_options_t : public options_interface_t 
{
    Q_OBJECT
    Q_INTERFACES(options_interface_t)

public:
    dr_heapstat_options_t(void);

    QStringList 
    tool_names(void) const;

    void
    set_options(options_t *options);

signals:
    void
    settings_changed(void);

private slots:
    void
    choose_def_load_dir(void);
    
private:
    void
    create_layout(void);

    void
    write_settings(void);

    void
    read_settings(void);

    options_t *options;

    /* GUI */
    QLineEdit *def_load_dir_line_edit;
    QCheckBox *ignore_first_snapshot_check_box;
    QCheckBox *square_graph_check_box;
    QCheckBox *antialiasing_check_box;
    QSpinBox *num_tabs_spin_box;
    QSpinBox *num_callstacks_per_page_spin_box;
};

#endif
