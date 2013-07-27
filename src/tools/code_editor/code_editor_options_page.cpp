/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: code_editor_options.cpp
** 
** Provides the code_editor options page
**
*************************************************************************/

#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSettings>
#include <QLabel>
#include <QDebug>
#include <QFileDialog>

#include "code_editor_options_page.h"

/* Public
 * Constructor
 */
code_editor_options_page_t::code_editor_options_page_t(void) 
{
    create_layout();
}

/* Public
 * Returns provided tool-names during loading
 */
QStringList 
code_editor_options_page_t::tool_names(void) const 
{
    return QStringList() << "Code Editor";
}

/* Public
 * Writes settings from the options struct
 */
void
code_editor_options_page_t::write_settings(void) 
{
    QSettings settings("Dynamorio", "DR-GUI");
    settings.beginGroup("Code_Editor");
    settings.setValue("pos", options->pos);
    settings.setValue("size", options->size);
    settings.endGroup();

    /* adjust info */
    read_settings();
    emit settings_changed();
}

/* Private
 * Reads settings into the options struct
 */
void
code_editor_options_page_t::read_settings(void) 
{
    if(options == NULL)
        return;
    QSettings settings("Dynamorio", "DR-GUI");
    settings.beginGroup("Code Editor");
    options->pos = settings.value("pos", QPoint(200, 200)).toPoint();
    options->size = settings.value("size", QSize(400, 400)).toSize();
    settings.endGroup();
}

/* Private
 * Reads settings into the options struct
 */
void
code_editor_options_page_t::set_options(options_t *options_) 
{
    qDebug() << "INFO: Entering code_editor_options_page_t::set_options"
                "(options_t *options_)";
    options = options_;
    read_settings();
}

/* Private
 * Creates and connects the GUI
 */
void
code_editor_options_page_t::create_layout(void) 
{
    QVBoxLayout *main_layout = new QVBoxLayout;

    QGroupBox *general_group = new QGroupBox(tr("General"), this);
    QVBoxLayout *general_layout = new QVBoxLayout;
    general_layout->addStretch(1);

    general_group->setLayout(general_layout);
    main_layout->addWidget(general_group);

    setLayout(main_layout);
}