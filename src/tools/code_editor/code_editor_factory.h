/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: code_editor_factory.h
** 
** Defines the code_editor factory
**
*************************************************************************/

#ifndef CODE_EDITOR_FACTORY_H
#define CODE_EDITOR_FACTORY_H

#include "tools/drgui_tool_interface.h"

class code_editor_win_t;
class code_editor_options_page_t;
struct options_t;

class code_editor_factory_t : public drgui_tool_interface_t 
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DrGUI_ToolInterface_iid FILE "code_editor.json")
    Q_INTERFACES(drgui_tool_interface_t)

public:
    code_editor_factory_t(void);

    ~code_editor_factory_t(void);

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
    QVector<code_editor_win_t *> tool_instances;

    /* Options */
    code_editor_options_page_t *options_page;
    options_t *options;
};

#endif