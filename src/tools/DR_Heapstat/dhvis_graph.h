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

/* dhvis_graph.h
 * 
 * Defines the Dr. Heapstat heap data graph
 */

#ifndef DHVIS_GRAPH_H
#define DHVIS_GRAPH_H

#include <QWidget>
#include <QPicture>

struct dhvis_options_t;
struct dhvis_snapshot_listing_t;

class dhvis_graph_t : public QWidget 
{
    Q_OBJECT
public:
    dhvis_graph_t(QVector<dhvis_snapshot_listing_t *> *vec,
                  dhvis_options_t *options_);
    
    bool
    is_null(void);

public slots:
    void 
    refresh_lines(int line, bool state);
    
    void 
    reset_graph_zoom(void);

    void
    update_settings();

signals:
    void 
    highlight_changed(int new_snapshot);

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
    highlighted_snapshot(void);
    
    void 
    set_heap_data(QVector<dhvis_snapshot_listing_t *> *vec);
    
    void 
    draw_empty_graph(QPainter *painter);
    
    void 
    draw_y_axis(QPainter *painter);
    
    void 
    draw_x_axis(QPainter *painter);
    
    void 
    draw_helper(QPainter *painter, qreal &total_percent, 
                QPoint *prev_point, qreal *data,
                bool first_point);
    
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

    /* Data */
    QVector<dhvis_snapshot_listing_t *> *snapshots;

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
    qreal view_start_percent;
    qreal view_end_percent;

    /* snapshot viewing */
    QPoint highlighted_point;
    qreal highlight_percent;
    int current_snapshot_num;

    /* lines */
    QPicture current_graph;
    bool current_graph_modified;
    bool mem_alloc_line;
    bool padding_line;
    bool headers_line;

    /* Options */
    dhvis_options_t *options;
};

#endif
