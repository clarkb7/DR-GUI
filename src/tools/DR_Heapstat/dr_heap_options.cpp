/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: dr_heap_options.cpp
** 
** Provides the DR. Heapstat options page
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

#include "dr_heap_options.h"

/* Public
 * Constructor
 */
dr_heapstat_options_t::dr_heapstat_options_t(void) 
{
    create_layout();
}

/* Public
 * Returns provided tool-names during loading
 */
QStringList 
dr_heapstat_options_t::tool_names(void) const 
{
    return QStringList() << "Dr. Heapstat";
}

/* Public
 * Writes settings from the options struct
 */
void
dr_heapstat_options_t::write_settings(void) 
{
    QSettings settings("Dynamorio", "DR-GUI");
    settings.beginGroup("Dr. Heapstat");
    settings.setValue("Default load directory", 
                      def_load_dir_line_edit->text());
    settings.setValue("Hide first snapshot", 
                      ignore_first_snapshot_check_box->isChecked() == true);
    settings.setValue("Square graph", 
                       square_graph_check_box->isChecked() == true);
    settings.setValue("Antialiasing", 
                       antialiasing_check_box->isChecked() == true);
    settings.setValue("Number of vertical ticks", num_tabs_spin_box->value());
    settings.setValue("Number of callstacks per page",  
                      num_callstacks_per_page_spin_box->value());
    settings.endGroup();

    /* adjust info */
    read_settings();
    emit settings_changed();
}

/* Private
 * Reads settings into the options struct
 */
void
dr_heapstat_options_t::read_settings(void) 
{
    if(options == NULL)
        return;
    QSettings settings("Dynamorio", "DR-GUI");
    settings.beginGroup("Dr. Heapstat");
    options->def_load_dir = settings.value("Default load directory",
                                           QString("/home")).toString();
    options->hide_first_snapshot = settings.value("Hide first snapshot", 
                                                  true).toBool();
    options->square_graph = settings.value("Square graph", false).toBool();
    options->antialiasing_enabled = settings.value("Antialiasing", 
                                                   true).toBool();
    options->num_vertical_ticks = settings.value("Number of vertical ticks", 
                                                 10).toInt();
    options->num_callstacks_per_page = settings.value(
                                               "Number of callstacks per page",
                                               50).toInt();
    settings.endGroup();

    /* adjust GUI to reflect new settings */
    def_load_dir_line_edit->setText(options->def_load_dir);
    ignore_first_snapshot_check_box->setChecked(options->hide_first_snapshot
                                                == true);
    square_graph_check_box->setChecked(options->square_graph == true);
    antialiasing_check_box->setChecked(options->antialiasing_enabled == true);
    num_tabs_spin_box->setValue(options->num_vertical_ticks);
    num_callstacks_per_page_spin_box->setValue(options->
                                                   num_callstacks_per_page);
}

/* Private
 * Reads settings into the options struct
 */
void
dr_heapstat_options_t::set_options(options_t *options_) 
{
    qDebug() << "INFO: Entering dr_heapstat_options_t::set_options"
                "(options_t *options_)";
    options = options_;
    read_settings();
}

/* Private
 * Creates and connects the GUI
 */
void
dr_heapstat_options_t::create_layout(void) 
{
    QGroupBox *general_group = new QGroupBox(tr("General"), this);
    QLabel *load_dir_label = new QLabel(tr("Default loading directory:"));
    def_load_dir_line_edit = new QLineEdit(this);
    QPushButton *find_def_load_dir_button = new QPushButton(tr("Select"));
    connect(find_def_load_dir_button, SIGNAL(clicked()),
            this, SLOT(choose_def_load_dir()));
    QGroupBox *graph_group = new QGroupBox(tr("Graph"), this);
    ignore_first_snapshot_check_box = new QCheckBox(tr("Ignore first "
                                                       "snapshot"));
    square_graph_check_box = new QCheckBox(tr("Square graph"));
    antialiasing_check_box = new QCheckBox(tr("Antialiasing"));

    num_tabs_spin_box = new QSpinBox(this);
    QLabel *tabs_spin_box_label = new QLabel(tr(" vertical scale ticks"));
    num_tabs_spin_box->setMinimum(1);
    
    num_callstacks_per_page_spin_box = new QSpinBox(this);
    QLabel *callstack_spin_box_label = new QLabel(tr(" callstacks per page"));
    num_callstacks_per_page_spin_box->setMinimum(1);
    num_callstacks_per_page_spin_box->setMaximum(500);

    QVBoxLayout *main_layout = new QVBoxLayout;

    QGridLayout *general_layout = new QGridLayout;
    general_layout->addWidget(load_dir_label, 0, 0);
    general_layout->addWidget(def_load_dir_line_edit, 1, 0);
    general_layout->addWidget(find_def_load_dir_button, 1, 1);
    general_group->setLayout(general_layout);
    
    QGridLayout *graph_layout = new QGridLayout;
    graph_layout->addWidget(ignore_first_snapshot_check_box, 0, 0);
    graph_layout->addWidget(square_graph_check_box, 1, 0);
    graph_layout->addWidget(antialiasing_check_box, 2, 0);
    graph_layout->addWidget(num_tabs_spin_box, 3, 0);
    graph_layout->addWidget(tabs_spin_box_label, 3, 1);
    graph_layout->addWidget(num_callstacks_per_page_spin_box, 4, 0);    
    graph_layout->addWidget(callstack_spin_box_label, 4, 1);
    graph_group->setLayout(graph_layout);

    main_layout->addWidget(general_group);
    main_layout->addWidget(graph_group);
    main_layout->addStretch(1);

    setLayout(main_layout);
}

/* Private Slot
 * User chooses def_load_dir
 */
void
dr_heapstat_options_t::choose_def_load_dir(void)
{
   QString test_dir;
   test_dir = QFileDialog::getExistingDirectory(this, 
                                                tr("Open Directory"),
                                                options->def_load_dir, 
                                                QFileDialog::ShowDirsOnly);
   if (test_dir.isEmpty() == true) {
       return;
   }
   /* set text box text */
   def_load_dir_line_edit->setText(test_dir);
}