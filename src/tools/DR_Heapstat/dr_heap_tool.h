/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: dr_heap_tool.h
** 
** Defines the DR. Heapstat tool
**
*************************************************************************/

#ifndef DR_HEAP_TOOL_H
#define DR_HEAP_TOOL_H

#include "tools/toolbase.h"

class QGraphicsView;
class QGraphicsScene;
class QDir;
class QFile;
class QTableWidget;
class QTextEdit;
class QPushButton;
class QLineEdit;
class QGridLayout;

struct snapshot_listing {
    QVector<int> assoc_callstacks;
    int snapshot_num;
    unsigned long tot_mallocs,
                  tot_bytes_asked_for,
                  tot_bytes_usable,
                  tot_bytes_occupied,
                  num_ticks;
};

class dr_heapstat_graph_t;

class dr_heapstat_t : public tool_base_t,
                      public tool_interface_t 
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.DR-GUI.ToolInterface" FILE "DR_Heapstat.json")
    Q_INTERFACES(tool_interface_t)

public:
    dr_heapstat_t();

    ~dr_heapstat_t();

    QStringList 
    tool_names(void) const;

    dr_heapstat_t *
    create_instance();

private slots:
    void 
    load_results(void);

    void 
    fill_callstacks_table(int snapshot);

    void 
    log_dir_text_changed_slot(void);

    void 
    load_settings(void);

    void 
    load_frames_text_edit(int current_row, int current_column, 
                          int previous_row, int previous_column);

    void 
    draw_snapshot_graph(void);

    void 
    change_lines(void);

private:
    void 
    create_actions(void);

    void 
    create_layout(void);

    bool 
    dr_check_dir(QDir dir);

    bool 
    dr_check_file(QFile& file);

    void 
    read_log_data(void);

    /* GUI */
    QGraphicsView *graph_view;
    QGraphicsScene *graph_scene;

    QTableWidget *callstacks_table;
    QTextEdit *frames_text_edit;
    QPushButton *prev_frame_button;
    QPushButton *next_frame_button;

    QLineEdit *log_dir_line_edit;
    bool log_dir_text_changed;
    QPushButton *load_results_button;

    QString log_dir_loc;

    QGridLayout *left_side;
    dr_heapstat_graph_t *snapshot_graph;
    QPushButton *reset_graph_zoom_button;

    /* Data */
    struct callstack_listing {
        QStringList frame_data;
        unsigned long  callstack_num,
                       instances, 
                       bytes_asked_for,
                       extra_usable,
                       extra_occupied;
    };
    QVector<struct callstack_listing*> callstacks;
    QVector<struct snapshot_listing*> snapshots;
};

#endif
