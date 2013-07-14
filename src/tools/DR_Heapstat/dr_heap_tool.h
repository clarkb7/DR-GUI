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
class QTabWidget;
class QTreeWidget;
class QTreeWidgetItem;

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
    draw_snapshot_graph(void);

    void 
    change_lines(void);

    void
    show_prev_page(void);

    void
    show_next_page(void);

    void
    refresh_frame_views(int current_row, int current_column, 
                        int previous_row, int previous_column);
    void
    frames_tree_widget_double_clicked(QTreeWidgetItem *item, int column);

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

    void 
    load_frames_text_edit(int current_row);

    void 
    load_frames_tree_widget(int current_row);

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

    QHBoxLayout *callstacks_page_buttons;
    QPushButton *prev_page_button;
    QLabel *page_display_label;
    QPushButton *next_page_button;
    
    QTabWidget *frames_tab_area;
    QTextEdit *frames_text_edit;

    QWidget *frames_tree_tab_widget;
    QVBoxLayout *frames_tree_layout;
    QTreeWidget *frames_tree_widget;

    QHBoxLayout *frames_tree_controls_layout;
    QPushButton *expand_all_button;
    QPushButton *collapse_all_button;

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
