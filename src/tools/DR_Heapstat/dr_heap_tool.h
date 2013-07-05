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

#include <QWidget>

class QGraphicsView;
class QGraphicsScene;
class QDir;
class QFile;
class QTableWidget;
class QTextEdit;
class QPushButton;
class QLineEdit;
class QGridLayout;

class dr_heapstat_graph_t;
struct options_t;
struct snapshot_listing;
struct callstack_listing;

class dr_heapstat_t : public QWidget
{
    Q_OBJECT

public:
    dr_heapstat_t(options_t *options_);

    ~dr_heapstat_t(void);

    void
    update_settings(void);

private slots:
    void 
    load_results(void);

    void 
    fill_callstacks_table(int snapshot);

    void 
    log_dir_text_changed_slot(void);

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
    QVector<callstack_listing *> callstacks;
    QVector<snapshot_listing *> snapshots;

    /* Options */
    options_t *options;
};

#endif
