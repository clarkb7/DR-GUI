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
class QHBoxLayout;
class QLabel;
class QVBoxLayout;
class QCheckBox;

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

    void
    show_prev_frame(void);

    void
    show_next_frame(void);

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
    QGridLayout *main_layout;

    QHBoxLayout *controls_layout;
    QLineEdit *log_dir_line_edit;
    bool log_dir_text_changed;
    QPushButton *load_results_button;

    QGridLayout *left_side;
    QLabel *graph_title;
    dr_heapstat_graph_t *snapshot_graph;
    QPushButton *reset_graph_zoom_button;

    QVBoxLayout *check_box_layout;
    QCheckBox *headers_check_box;
    QCheckBox *padding_check_box;
    QCheckBox *mem_alloc_check_box;

    QVBoxLayout *msg_layout;
    QLabel *msg_title;
    QTextEdit *messages;

    QGridLayout *right_side;
    QLabel *right_title;
    QTableWidget *callstacks_table;

    QHBoxLayout *frame_buttons;
    QPushButton *prev_frame_button;
    QLabel *display_label;
    QPushButton *next_frame_button;
    
    QTextEdit *frames_text_edit;

    QString log_dir_loc;
    
    /* Data */
    QVector<callstack_listing *> callstacks;
    QVector<snapshot_listing *> snapshots;
    int callstacks_display_page;
    int current_snapshot_num;

    /* Options */
    options_t *options;
};

#endif
