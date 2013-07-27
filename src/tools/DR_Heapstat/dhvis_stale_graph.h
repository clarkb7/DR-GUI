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

/* dhvis_stale_graph.h
 * 
 * Defines the Dr. Heapstat staleness data graph
 */

#ifndef DHVIS_STALE_GRAPH_H
#define DHVIS_STALE_GRAPH_H

#include <QWidget>
#include <QPicture>

class QHBoxLayout;
class QLabel;
class QPushButton;

struct dhvis_callstack_listing_t;
struct dhvis_snapshot_listing_t;
struct dhvis_options_t;

class dhvis_stale_graph_t : public QWidget 
{
    Q_OBJECT
public:
    dhvis_stale_graph_t(QVector<dhvis_callstack_listing_t *> *vec,
                        dhvis_snapshot_listing_t *snapshot_,
                        QString *time_unit_,
                        dhvis_options_t *options_);
    
    bool
    is_null(void);

public slots:
    void
    update_settings();

private slots:
    void 
    reset_graph_zoom(void);

protected:
    void 
    paintEvent(QPaintEvent *event);
    
    void 
    resizeEvent(QResizeEvent *event);
    
    void 
    mousePressEvent(QMouseEvent *event);
    
    void 
    mouseReleaseEvent(QMouseEvent *event);
    
    void 
    mouseMoveEvent(QMouseEvent *event);

private:    
    void 
    set_heap_data(QVector<dhvis_callstack_listing_t *> *vec,
                  dhvis_snapshot_listing_t *snapshot_);

    void 
    draw_empty_graph(QPainter *painter);
    
    void 
    draw_y_axis(QPainter *painter);
    
    void 
    draw_x_axis(QPainter *painter);
    
    void 
    draw_helper(QPainter *painter, qreal &total_percent, 
                qreal &prev_percent,
                dhvis_callstack_listing_t *callstack);
    
    void 
    draw_heap_data(QPainter *painter);
    
    void 
    draw_selection(QPainter *painter);
    
    void
    max_height(void);
    
    void 
    max_width(void);
    
    qreal 
    y_axis_width(void);
    
    qreal 
    x_axis_height(void);
    
    qreal 
    data_point_y(qreal y);
    
    qreal 
    data_point_x(qreal x);

    void
    draw_view_cursor(QPainter *painter);
    
    qreal
    calc_callstack_index(qreal x_val);
    
    /* Data */
    QVector<dhvis_callstack_listing_t*> *callstacks;
    dhvis_snapshot_listing_t *snapshot;
    QString *time_unit;

    /* Graph Boundaries */
    qreal graph_outer_margin;
    QString maximum_value;
    qreal width_max;
    qreal height_max;
    qreal text_width;
    qreal text_height;
    qreal left_bound;
    qreal right_bound;

    /* Selection/zoom */
    bool mouse_pressed;
    QPoint first_point;
    QPoint last_point;
    qreal view_start_index;
    qreal view_end_index;

    /* snapshot viewing */
    QPoint highlighted_point;
    qreal highlight_percent;
    int current_snapshot_num;

    /* Graph */
    QPicture current_graph;
    bool current_graph_modified;

    /* Controls */
    QHBoxLayout *control_layout;
    QLabel *view_info_label;
    QLabel *zoom_info_label;
    QPushButton *reset_zoom_button;

    /* Options */
    dhvis_options_t *options;
};

#endif
