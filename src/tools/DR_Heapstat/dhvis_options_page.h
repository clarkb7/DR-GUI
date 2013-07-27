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

/* dhvis_options_page.h
 * 
 * Defines the DR. Heapstat options page
 */

#ifndef DHVIS_OPTIONS_H
#define DHVIS_OPTIONS_H

#include "tools/drgui_options_interface.h"
#include "dhvis_structures.h"

class QSpinBox;
class QLineEdit;
class QCheckBox;

class dhvis_options_page_t : public drgui_options_interface_t 
{
    Q_OBJECT
    Q_INTERFACES(drgui_options_interface_t)

public:
    dhvis_options_page_t(void);

    QStringList 
    tool_names(void) const;

    void
    set_options(dhvis_options_t *options);

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

    dhvis_options_t *options;

    /* GUI */
    QLineEdit *def_load_dir_line_edit;
    QCheckBox *hide_peak_snapshot_check_box;
    QCheckBox *square_graph_check_box;
    QCheckBox *antialiasing_check_box;
    QSpinBox *num_tabs_spin_box;
    QSpinBox *num_callstacks_per_page_spin_box;
};

#endif
