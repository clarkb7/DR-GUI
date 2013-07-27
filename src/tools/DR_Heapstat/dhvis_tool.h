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

/* dhvis_tool.h
 * 
 * Defines the DR. Heapstat tool
 */

#ifndef DHVIS_TOOL_H
#define DHVIS_TOOL_H

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

class dhvis_graph_t;
class dhvis_stale_graph_t;
struct dhvis_options_t;
struct dhvis_snapshot_listing_t;
struct dhvis_callstack_listing_t;

class dhvis_tool_t : public QWidget
{
    Q_OBJECT

public:
    dhvis_tool_t(dhvis_options_t *options_);

    ~dhvis_tool_t(void);

    void
    update_settings(void);

private slots:
    void 
    load_results(void);

    void 
    log_dir_text_changed_slot(void);

    void 
    draw_snapshot_graph(void);
    
    void
    draw_staleness_graph(void);

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

    void
    highlight_changed(int snapshot);

signals:
    void
    code_editor_requested(QFile &file, int line_num);

private:
    void 
    create_layout(void);

    bool 
    dr_check_dir(QDir dir);

    bool 
    dr_check_file(QFile& file);

    void 
    read_log_data(void);

    void 
    fill_callstacks_table(void);

    void 
    load_frames_text_edit(int current_row);

    void 
    load_frames_tree_widget(void);

    /* GUI */
    QGridLayout *main_layout;

    QHBoxLayout *controls_layout;
    QLineEdit *log_dir_line_edit;
    bool log_dir_text_changed;
    QPushButton *load_results_button;

    QGridLayout *left_side;
    QLabel *graph_title;
    dhvis_graph_t *snapshot_graph;
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

    dhvis_stale_graph_t *staleness_graph;

    QString log_dir_loc;
    
    /* Data */
    QVector<dhvis_callstack_listing_t *> callstacks;
    QVector<dhvis_snapshot_listing_t *> snapshots;
    QString time_unit;
    int callstacks_display_page;
    int current_snapshot_num;

    /* Options */
    dhvis_options_t *options;
};

#endif
