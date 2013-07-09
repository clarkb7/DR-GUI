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
** Provides the Dr. Heapstat heap data graph
**
*************************************************************************/

#define GRAPH_MARK_WIDTH 5

#include <QWidget>
#include <QPainter>
#include <QPicture>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QMouseEvent>

#include <cmath>

#include "dr_heap_structures.h"
#include "dr_heap_tool.h"
#include "dr_heap_graph.h"

/* Public
 * Constructor
 */
dr_heapstat_graph_t::dr_heapstat_graph_t(QVector<snapshot_listing *> *vec,
                                         options_t *options_)
    :  snapshots(NULL), graph_outer_margin(10), options(options_)
{
    qDebug() << "INFO: Entering dr_heapstat_graph_t::dr_heapstat_graph_t"
                "(QVector<struct snapshot_listing*> *vec)";
    setAttribute(Qt::WA_DeleteOnClose);
    view_start_percent = 0;
    view_end_percent = 100;
    current_snapshot_num = 1;
    highlighted_point = QPoint(0,0);
    current_graph_modified = mem_alloc_line = padding_line 
                           = headers_line = true;
    set_heap_data(vec);
}

/* Private
 * Sets heap data to be visualized
 */
void 
dr_heapstat_graph_t::set_heap_data(QVector<struct snapshot_listing *> *vec) 
{
    qDebug() << "INFO: Entering dr_heapstat_graph_t::set_heap_data"
                "(QVector<struct snapshot_listing*> *vec)";
    /* memory should be taken care of by tool */
    if (vec != NULL) {
        snapshots = vec;        
    }
    max_height();
    max_width();

    QFontMetrics fm(font());
    text_height = fm.height();
    text_width = fm.width(maximum_value);

    left_bound = graph_outer_margin + text_width + 5;
    right_bound = left_bound + width() - y_axis_width() -
                  2 * graph_outer_margin;   

    update();
}

/* Protected
 * Paints an empty canvis or loads data
 */
void 
dr_heapstat_graph_t::paintEvent(QPaintEvent *event) 
{
    qDebug() << "INFO: Entering dr_heapstat_graph_t::paintEvent"
                "(QPaintEvent *event)";
    QWidget::paintEvent(event);

    QPainter painter(this);

    /* Fix origin location */
    painter.translate(left_bound,
                       height() - graph_outer_margin);
    painter.scale(1,-1);

    if (snapshots == NULL || snapshots->isEmpty() == true)
        draw_empty_graph(&painter);
    else {
        if(current_graph_modified == true) {
            QPainter data_painter(&current_graph);
            draw_x_axis(&data_painter);
            draw_y_axis(&data_painter);
            draw_heap_data(&data_painter);
            data_painter.end();
            current_graph_modified = false;
            highlighted_snapshot();
        }
        painter.drawPicture(0,0,current_graph);
        draw_selection(&painter);
        draw_view_cursor(&painter);
    }
}

/* Private
 * Calculates max height of y-axis
 */
void 
dr_heapstat_graph_t::max_height(void) 
{
    qreal height = 0;
    if (snapshots != NULL &&
        snapshots->isEmpty() == false) {
        foreach(snapshot_listing *snapshot, *snapshots) {
            if (options != NULL &&
                options->hide_first_snapshot == true &&
                snapshot->snapshot_num == 0) {
                continue;
            }
            if (snapshot->tot_bytes_occupied > height)
                height = snapshot->tot_bytes_occupied;
        }
    }
    maximum_value = QString::number(height);
    height_max = height;
}

/* Private
 * Calculates max width of x-axis
 */
void 
dr_heapstat_graph_t::max_width(void) 
{
    qreal width = 0;
    if (snapshots != NULL &&
        snapshots->isEmpty() == false) {
        foreach(snapshot_listing *snapshot, *snapshots) {
            if (options != NULL &&
                options->hide_first_snapshot == true &&
                snapshot->snapshot_num == 0) {
                continue;
            }
            width += snapshot->num_ticks;
        }
    }
    width_max = width;
}

/* Private
 * Returns width of y_axis
 */
qreal 
dr_heapstat_graph_t::y_axis_width(void) 
{
    return text_width + 5;
}

/* Private
 * Returns height of x_axis
 */
/* XXX: no text on x-axis, probably don't need */
qreal 
dr_heapstat_graph_t::x_axis_height(void) 
{
    return text_height + 5;
}

/* Private
 * Calculates x-coord for given data
 */
qreal 
dr_heapstat_graph_t::data_point_x(qreal x) 
{
    int max_x = width() - y_axis_width() - 2 * graph_outer_margin;
    return x * (max_x) / (double)(view_end_percent - view_start_percent);
}

/* Private
 * Calculates y-coord for given data
 */
qreal 
dr_heapstat_graph_t::data_point_y(qreal y) 
{
    int max_y = height() - x_axis_height() - 2 * graph_outer_margin;
    return y * (max_y) / maximum_value.toULong();
}

/* Private
 * Draws an empty graph when no data is present
 */
void 
dr_heapstat_graph_t::draw_empty_graph(QPainter *painter) 
{
    qDebug() << "INFO: Entering dr_heapstat_graph_t::draw_empty_graph"
                "(QPainter *painter)";
    QString msg("No datapoints available!");

    qreal center_x = width() / 2;
    qreal center_y = height() / 2;

    QFontMetricsF font_metrics(font());
    qreal msg_width = font_metrics.width(msg);
    qreal msg_height = font_metrics.height();

    qreal msg_x = center_x - (msg_width / 2);
    qreal msg_y = center_y - (msg_height / 2);
    
    /* fix scale for text, it appears upside down */
    painter->save();
    painter->scale(1,-1);
    painter->drawText(QPointF(msg_x, msg_y), msg);
    painter->restore();
}

/* Private
 *  Draws the x-axis
 */
void 
dr_heapstat_graph_t::draw_x_axis(QPainter *painter) 
{
   qDebug() << "INFO: Entering dr_heapstat_graph_t::draw_x_axis"
               "(QPainter *painter)";
    painter->save();

    QPen x_axis_pen(QColor(qRgb(0, 0, 0)));

    qreal x_axis_x = 0;
    qreal x_axis_y = 0;
    qreal x_axis_width = right_bound - left_bound;

    painter->setPen(x_axis_pen);
    painter->drawLine(QPointF(x_axis_x, x_axis_y), 
                      QPointF(x_axis_width, x_axis_y));

    qreal x_axis_mark = x_axis_x;
    qreal mark_width;
    qreal percent_diff = (view_end_percent - view_start_percent);
    /* floating point exception check */
    if (percent_diff < 2.0)
        percent_diff = 2.0;
    /* Draw tallies based on % */
    for(qreal i = 0; i <= percent_diff; i++) {
        mark_width = x_axis_y - GRAPH_MARK_WIDTH;
        if ((int)i % (int)round(percent_diff / 4.0) == 0) 
            mark_width -= 2;
        painter->drawLine(QPointF(x_axis_mark, x_axis_y), 
                          QPointF(x_axis_mark, mark_width));

        /* Adjust count */
        x_axis_mark += (x_axis_width-x_axis_x)
                        / percent_diff;
    }

    painter->restore();
}

/* Private
 * draws y-axis and scale labels
 */
void 
dr_heapstat_graph_t::draw_y_axis(QPainter *painter) 
{
    qDebug() << "INFO: Entering dr_heapstat_graph_t::draw_y_axis"
                "(QPainter *painter)";
    painter->save();

    QPen y_axis_pen(QColor(qRgb(0, 0, 0)));

    qreal y_axis_x = 0;
    qreal y_axis_y = 0;
    qreal y_axis_height = height() - y_axis_y -
                          x_axis_height() - 2 * graph_outer_margin;

    painter->setPen(y_axis_pen);
    painter->drawLine(QPointF(y_axis_x, y_axis_y), 
                      QPointF(y_axis_x, y_axis_height));

    /* Draw scale */
    qreal y_axis_mark = 0;
    qreal cur_value = 0;
    qreal max_val = height_max;
    int num_tabs = options->num_vertical_ticks;
    for (int count = 0; count <= num_tabs; count++) {
        /* Ensure max is correct */
        if (count == num_tabs) {
            cur_value = max_val;
            y_axis_mark = y_axis_height;
        }
        /* fix scale for text, it appears upside down */
        painter->save();
        painter->scale(1,-1);
        painter->drawText(QRectF(-(text_width + graph_outer_margin), 
                                 -(y_axis_mark + text_height/2),
                                 text_width,
                                 text_height),
                          QString::number(round(cur_value)),
                          QTextOption(Qt::AlignRight));
        painter->restore();
        painter->drawLine(QPointF(y_axis_x - GRAPH_MARK_WIDTH, y_axis_mark), 
                          QPointF(y_axis_x, y_axis_mark));

        /* Adjust counts */
        y_axis_mark += (y_axis_height - y_axis_y) / (double)num_tabs;
        cur_value += max_val / (double)num_tabs;
    }
    painter->restore();
}

/* Private
 * Helps draw_heap_data(...) graph data
 */
void 
dr_heapstat_graph_t::draw_helper(QPainter *painter, qreal total_percent, 
                                 QPoint *prev_point, qreal *data,
                                 bool first) 
{
    qreal dp_x = data_point_x(total_percent - view_start_percent);
    qreal dp_y = data_point_y(*data);
    
    /* Place first point at correct loc */
    if (first == true) {
        if(options->square_graph == false) {
            qreal slope = (dp_y - prev_point->y()) / 
                          (double)(dp_x - prev_point->x());
            prev_point->setY(slope * (0 - prev_point->x()) + prev_point->y());
        } else {
            prev_point->setY(dp_y);
        }
        prev_point->setX(0);
    }

    /* square graph */
    if(options->square_graph == true) {
        QPoint mid_point(prev_point->x(), dp_y);
        painter->drawLine(*prev_point, mid_point);
        painter->drawRect(prev_point->x() - 1.5, dp_y - 1.5, 3, 3);
        prev_point->setX(mid_point.x());
        prev_point->setY(mid_point.y());
    }



    QPoint this_point(dp_x, dp_y);
    painter->drawLine(*prev_point, this_point);
    painter->drawRect(dp_x - 1.5, dp_y - 1.5, 3, 3);
    prev_point->setX(dp_x);
    prev_point->setY(dp_y);
}

/* Private
 * Graphs data
 */
void 
dr_heapstat_graph_t::draw_heap_data(QPainter *painter) 
{
    qDebug() << "INFO: Entering dr_heapstat_graph_t::draw_heap_data"
                "(QPainter *painter)";
    painter->save();

    qreal total_percent = 0;
    /* need for each line */
    QVector<QPoint> prev_points;
    for(int i = 0; i < 3; i++) 
        prev_points.append(QPoint());
    /* TODO: use preference for color*/
    QBrush data_point_brush(Qt::red);
    QPen data_point_pen(Qt::white, 3, Qt::SolidLine, 
                        Qt::RoundCap, Qt::RoundJoin);

    if (options->antialiasing_enabled)
        painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(data_point_brush);
    painter->setPen(data_point_pen);

    bool first = true;
    bool last = false;
    foreach(snapshot_listing *snapshot, *snapshots) {
        if (options->hide_first_snapshot == true &&
            snapshot->snapshot_num == 0) {
            continue;
        }

        total_percent += (snapshot->num_ticks / ((double)width_max)) * 100;
        if (total_percent < view_start_percent) {
            prev_points.data()[0] = QPoint(data_point_x(total_percent -
                                                        view_start_percent),
                                           data_point_y(snapshot->
                                                         tot_bytes_asked_for));
            prev_points.data()[1] = QPoint(data_point_x(total_percent -
                                                        view_start_percent),
                                           data_point_y(snapshot->
                                                         tot_bytes_usable));
            prev_points.data()[2] = QPoint(data_point_x(total_percent -
                                                        view_start_percent),
                                           data_point_y(snapshot->
                                                         tot_bytes_occupied));
            continue;
        } 

        if (last == true) {
            break;
        } else if (total_percent > view_end_percent) {
            last = true;
        }

        if (mem_alloc_line == true) {
            data_point_pen.setColor(QColor(255, 102, 0));
            painter->setPen(data_point_pen);
            draw_helper(painter, total_percent, 
                        &prev_points.data()[0], 
                        &snapshot->tot_bytes_asked_for,
                        first);
        }
        if (padding_line == true) {
            data_point_pen.setColor(QColor(0, 204, 0));
            painter->setPen(data_point_pen);
            draw_helper(painter, total_percent, 
                        &prev_points.data()[1], 
                        &snapshot->tot_bytes_usable,
                        first);
        }
        if (headers_line == true) {
            data_point_pen.setColor(QColor(27, 168, 188));
            painter->setPen(data_point_pen);
            draw_helper(painter, total_percent, 
                        &prev_points.data()[2], 
                        &snapshot->tot_bytes_occupied,
                        first);
        }
        first = false;
    }
    painter->restore();
}

/* Private
 * Draws a rectangle for a selection zoom guide
 */
void 
dr_heapstat_graph_t::draw_selection(QPainter *painter) 
{
    painter->save();
    /* transparency == 40 */
    QColor selection_color(0, 203, 204, 40);
    painter->setPen(QPen(selection_color));
    painter->setBrush(QBrush(selection_color, Qt::SolidPattern));
    painter->drawRect(first_point.x(), -10,
                      last_point.x()-first_point.x(),last_point.y());
    painter->restore();
}

/* Protected
 * Interactivity for graph
 *   -Selection Zoom
 */
void 
dr_heapstat_graph_t::mousePressEvent(QMouseEvent *event) 
{
    qreal x_val = event->pos().x();
    /* Check bounds */
    if (x_val < left_bound) {
        x_val = left_bound;
    } else if (x_val > right_bound) {
        x_val = right_bound;
    }
    /* Handle event */
    if (event->button() == Qt::RightButton) {
        mouse_pressed = true;
        first_point = event->pos();  
        first_point.setX(x_val - left_bound);      
    }
}

/* Protected
 * Interactivity for graph
 *   -Selection Zoom
 */
void 
dr_heapstat_graph_t::mouseReleaseEvent(QMouseEvent *event) 
{
    if (event->button() == Qt::RightButton) {
        /* set vars to adjust graph
         * then adjust graph 
         */
        qreal temp_start = view_start_percent;
        qreal temp_end = view_end_percent;

        qreal bound_diff = (right_bound - left_bound);
        qreal percent_diff = (temp_end - temp_start);

        view_start_percent = temp_start + (first_point.x() / (bound_diff)) 
                                          * percent_diff;
        view_end_percent = temp_start + (last_point.x() / (bound_diff)) 
                                        * percent_diff;

        /* switch if user went right to left */                            
        if (first_point.x() > last_point.x()) {
            qreal temp = view_start_percent;
            view_start_percent = view_end_percent;
            view_end_percent = temp;
        }
        /* floating point exception with diff < 2 
         * From drawing elongated tallies in draw_x_axis
         * (i % (int)round(diff / 4))
         */
        if (abs(view_start_percent - view_end_percent) < 2.0000) {
            view_end_percent = view_start_percent + 2.0000;
        }
        /* limit */
        if (view_start_percent >= 98) {
            view_end_percent = 100;
            view_start_percent = 98;
        }
        current_graph_modified = true;
        /* reset selection info */
        mouse_pressed = false;
        first_point.setX(0);
        first_point.setY(0);
        last_point.setX(0);
        last_point.setY(0);
        update();
    }
}

/* Protected
 * Interactivity for graph
 *   -Selection Zoom
 *   -Snapshot Highlighting
 */
void 
dr_heapstat_graph_t::mouseMoveEvent(QMouseEvent *event) 
{
    qreal x_val = event->pos().x();
    /* Check bounds */
    if (event->pos().x() < left_bound) {
        x_val = left_bound;
    } else if (event->pos().x() > right_bound) {
        x_val = right_bound;
    }
    /* For selection zoom */
    if (event->buttons() & Qt::RightButton) {
        last_point = QPoint(x_val-left_bound,height());
    } /* for snapshot highlighting */ 
    else if (event->buttons() & Qt::LeftButton) {
        highlighted_point = event->pos();
        highlighted_point.setX(x_val-left_bound);
        highlighted_snapshot();
    }
        update();
}

/* Protected
 * Adjusts right_bound with change in widget size
 */
void 
dr_heapstat_graph_t::resizeEvent(QResizeEvent *event) 
{
    Q_UNUSED(event);
    /* dependent on width */
    right_bound = left_bound + width() - y_axis_width() - 
                  2 * graph_outer_margin;
    current_graph_modified = true;
}

/* Public slots
 * dis/en/ables line drawing
 */
void 
dr_heapstat_graph_t::refresh_lines(int line, bool state) 
{
    switch (line) {
        case 0:
            mem_alloc_line = state;
            break;
        case 1:
            padding_line = state;
            break;
        case 2:
            headers_line = state;
            break;
        default:
            break;
    }
    current_graph_modified = true;
    update();
}

/* Private
 * Finds which snapshot to highlight according to slider position
 */
void 
dr_heapstat_graph_t::highlighted_snapshot(void) 
{
    if(snapshots == NULL)
        return;
    qreal total_percent_lesser = view_start_percent;
    qreal total_percent_greater = view_start_percent;
    qreal total_percent = 0;
    qreal percent_diff = view_end_percent - view_start_percent;
    /* Cursor */
    qreal highlight_percent = view_start_percent + (highlighted_point.x() 
                              / (double)(right_bound-left_bound)) 
                              * percent_diff;
    highlight_percent = data_point_x(highlight_percent-view_start_percent);

    foreach(snapshot_listing *snapshot, *snapshots) {
        if (options->hide_first_snapshot == true &&
            snapshot->snapshot_num == 0)
            continue;
        /* Snapshot locs */
        total_percent += (snapshot->num_ticks 
                          / ((double)width_max)) * 100;
        total_percent_greater = data_point_x(total_percent 
                                              - view_start_percent);
        /* If found */
        if (highlight_percent >= total_percent_lesser &&
            highlight_percent <= total_percent_greater) {
            emit highlight_changed(snapshot->snapshot_num);
            current_snapshot_num = snapshot->snapshot_num;
            return;
        }
        total_percent_lesser = total_percent_greater;
    }
    return;
}

/* Public slots
 * resets the selection zoom on the graph
 */
void 
dr_heapstat_graph_t::reset_graph_zoom(void) 
{
    view_start_percent = 0;
    view_end_percent = 100;
    current_graph_modified = true;
    update();
}

/* Private
 * draws view cursor
 */
void
dr_heapstat_graph_t::draw_view_cursor(QPainter *painter) 
{
    painter->save();
    /* draw cursor line */
    painter->drawLine(QPoint(highlighted_point.x(),0),
                      QPoint(highlighted_point.x(),height()));
    /* draw snapshot num */
    /* fix scale for text, it appears upside down */
    painter->save();
    painter->scale(1,-1);
    painter->drawText(QRectF(highlighted_point.x() - 3 - text_width,
                             -(height() - 1.5 * graph_outer_margin),
                             text_width,
                             text_height),
                      "#" + QString::number(current_snapshot_num),
                      QTextOption(Qt::AlignRight));
    painter->restore();
    painter->restore();
}


void
dr_heapstat_graph_t::update_settings(void)
{
    current_graph_modified = true;
    set_heap_data(snapshots);
}

bool
dr_heapstat_graph_t::is_null(void) {
    return snapshots == NULL;
}