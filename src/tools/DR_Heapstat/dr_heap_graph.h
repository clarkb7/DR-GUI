/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: dr_heap_graph.cpp
** 
** Defines the Dr. Heapstat heap data graph
**
*************************************************************************/

#ifndef DR_HEAP_GRAPH_H
#define DR_HEAP_GRAPH_H

#include <QWidget>
#include <QPicture>

struct options_t;
struct snapshot_listing;

class dr_heapstat_graph_t : public QWidget 
{
    Q_OBJECT
public:
    dr_heapstat_graph_t(QVector<snapshot_listing *> *vec,
                        options_t *options_);
    
    bool
    is_null(void);

public slots:
    void 
    refresh_lines(int line);
    
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
    set_heap_data(QVector<snapshot_listing *> *vec);
    
    void 
    draw_empty_graph(QPainter *painter);
    
    void 
    draw_y_axis(QPainter *painter);
    
    void 
    draw_x_axis(QPainter *painter);
    
    void 
    draw_helper(QPainter *painter, qreal total_percent, 
               QPoint *prev_point, unsigned long *data);
    
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
    data_point_y(unsigned long y);
    
    qreal 
    data_point_x(qreal x);

    void
    draw_view_cursor(QPainter *painter);

    /* Data */
    QVector<snapshot_listing*> *snapshots;

    /* Graph Boundaries */
    qreal graph_outer_margin;
    QString maximum_value;
    unsigned long width_max;
    unsigned long height_max;
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
    int current_snapshot_num;

    /* lines */
    QPicture current_graph;
    bool current_graph_modified;
    bool mem_alloc_line;
    bool padding_line;
    bool headers_line;

    /* Options */
    options_t *options;
};

#endif
