/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: code_editor_options.h
** 
** Defines the code_editor options page
**
*************************************************************************/

#ifndef CODE_EDITOR_OPTIONS_H
#define CODE_EDITOR_OPTIONS_H

#include "tools/drgui_options_interface.h"
#include "code_editor_structures.h"

class QSpinBox;
class QLineEdit;
class QCheckBox;

class code_editor_options_page_t : public drgui_options_interface_t 
{
    Q_OBJECT
    Q_INTERFACES(drgui_options_interface_t)

public:
    code_editor_options_page_t(void);

    QStringList 
    tool_names(void) const;

    void
    set_options(options_t *options);

signals:
    void
    settings_changed(void);
    
private:
    void
    create_layout(void);

    void
    write_settings(void);

    void
    read_settings(void);

    options_t *options;

    /* GUI */

};

#endif
