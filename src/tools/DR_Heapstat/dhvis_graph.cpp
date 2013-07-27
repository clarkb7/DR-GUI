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

/* dhvis_graph.cpp
 * 
 * Provides the Dr. Heapstat heap data graph
 */

#ifdef __CLASS__
#  undef __CLASS__
#endif
#define __CLASS__ "dhvis_graph_t::"

#define GRAPH_MARK_WIDTH 5
/* Fix scale for text, it appears upside down */
#define DHVIS_DRAW_TEXT(command) painter->save(); \
                                 painter->scale(1, -1); \
                                 command; \
                                 painter->restore();
#include <QWidget>
#include <QPainter>
#include <QPicture>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QMouseEvent>

#include <cmath>

#include "dhvis_structures.h"
#include "dhvis_tool.h"
#include "dhvis_graph.h"

/* Public
 * Constructor
 */
dhvis_graph_t::dhvis_graph_t(QVector<dhvis_snapshot_listing_t *> 
                                         *vec,
                                         dhvis_options_t *options_)
    :  snapshots(NULL), graph_outer_margin(10), options(options_)
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    setAttribute(Qt::WA_DeleteOnClose);
    view_start_percent = 0;
    view_end_percent = 100;
    current_snapshot_num = -1;
    highlighted_point = QPoint(0,0);
    current_graph_modified = mem_alloc_line = padding_line 
                           = headers_line = true;
    set_heap_data(vec);
}

/* Private
 * Sets heap data to be visualized
 */
void 
dhvis_graph_t::set_heap_data(QVector<dhvis_snapshot_listing_t *> *vec) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    /* Memory should be taken care of by tool */
    if (vec != NULL)
        snapshots = vec;        
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
dhvis_graph_t::paintEvent(QPaintEvent *event) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    QWidget::paintEvent(event);

    QPainter painter(this);

    /* Fix origin location */
    painter.translate(left_bound,
                       height() - graph_outer_margin);
    painter.scale(1, -1);

    if (snapshots == NULL || snapshots->isEmpty())
        draw_empty_graph(&painter);
    else {
        if (current_graph_modified) {
            QPainter data_painter(&current_graph);
            draw_x_axis(&data_painter);
            draw_y_axis(&data_painter);
            draw_heap_data(&data_painter);
            data_painter.end();
            current_graph_modified = false;
        }
        painter.drawPicture(0, 0, current_graph);
        draw_selection(&painter);
        draw_view_cursor(&painter);
    }
}

/* Private
 * Calculates max height of y-axis
 */
void 
dhvis_graph_t::max_height(void) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    qreal height = 0;
    if (snapshots != NULL &&
        !snapshots->isEmpty()) {
        foreach (dhvis_snapshot_listing_t *snapshot, *snapshots) {
            if (options != NULL &&
                options->hide_peak_snapshot &&
                snapshot->is_peak) {
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
dhvis_graph_t::max_width(void) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    qreal width = 0;
    if (snapshots != NULL &&
        !snapshots->isEmpty()) {
        foreach (dhvis_snapshot_listing_t *snapshot, *snapshots) {
            if (options != NULL &&
                options->hide_peak_snapshot &&
                snapshot->is_peak) {
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
dhvis_graph_t::y_axis_width(void) 
{
    return text_width + 5;
}

/* Private
 * Returns height of x_axis
 */
/* XXX: no text on x-axis, probably don't need */
qreal 
dhvis_graph_t::x_axis_height(void) 
{
    return text_height + 5;
}

/* Private
 * Calculates x-coord for given data
 */
qreal 
dhvis_graph_t::data_point_x(qreal x) 
{
    qreal max_x = width() - y_axis_width() - 2 * graph_outer_margin;
    return x * (max_x) / (double)(view_end_percent - view_start_percent);
}

/* Private
 * Calculates y-coord for given data
 */
qreal 
dhvis_graph_t::data_point_y(qreal y) 
{
    qreal max_y = height() - x_axis_height() - 2 * graph_outer_margin;
    return y * (max_y) / maximum_value.toULong();
}

/* Private
 * Draws an empty graph when no data is present
 */
void 
dhvis_graph_t::draw_empty_graph(QPainter *painter) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    QString msg("No datapoints available!");

    qreal center_x = width() / 2;
    qreal center_y = height() / 2;

    QFontMetricsF font_metrics(font());
    qreal msg_width = font_metrics.width(msg);
    qreal msg_height = font_metrics.height();

    qreal msg_x = center_x - (msg_width / 2);
    qreal msg_y = center_y - (msg_height / 2);
    
    DHVIS_DRAW_TEXT(painter->drawText(QPointF(msg_x, -msg_y), msg))
}

/* Private
 * Draws the x-axis
 */
void 
dhvis_graph_t::draw_x_axis(QPainter *painter) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
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
    /* Floating point exception check */
    if (percent_diff < 2.0)
        percent_diff = 2.0;
    /* Draw tallies based on % */
    for (qreal i = 0; i <= percent_diff; i++) {
        mark_width = x_axis_y - GRAPH_MARK_WIDTH;
        if ((int)i % (int)round(percent_diff / 4.0) == 0) 
            mark_width -= 2;
        painter->drawLine(QPointF(x_axis_mark, x_axis_y), 
                          QPointF(x_axis_mark, mark_width));

        /* Adjust count */
        x_axis_mark += (x_axis_width-x_axis_x) / percent_diff;
    }

    painter->restore();
}

/* Private
 * Draws y-axis and scale labels
 */
void 
dhvis_graph_t::draw_y_axis(QPainter *painter) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
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
        QRectF text_space(-(text_width + graph_outer_margin), 
                          -(y_axis_mark + text_height / 2),
                          text_width,
                          text_height);
        DHVIS_DRAW_TEXT(painter->drawText(text_space,
                                          QString::number(round(cur_value)),
                                          QTextOption(Qt::AlignRight)))
        /* Cross-graph line 
         * save since diff color
         */
        painter->save();
        painter->setPen(QColor(0,0,0,25));
        painter->drawLine(QPointF(y_axis_x, y_axis_mark), 
                          QPointF(right_bound - left_bound, y_axis_mark));
        painter->restore();
        /* Axis tick */
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
dhvis_graph_t::draw_helper(QPainter *painter, qreal &total_percent, 
                           QPoint *prev_point, qreal *data,
                           bool first) 
{
    qreal dp_x = data_point_x(total_percent - view_start_percent);
    qreal dp_y = data_point_y(*data);
    
    /* Place first point at correct loc */
    if (first) {
        if(!options->square_graph) {
            qreal slope = (dp_y - prev_point->y()) / 
                          (double)(dp_x - prev_point->x());
            prev_point->setY(slope * (0 - prev_point->x()) + prev_point->y());
        } else {
            prev_point->setY(dp_y);
        }
        prev_point->setX(0);
    }

    /* Square graph */
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
#define DHVIS_MAKE_PREV_POINTS(loc, num) \
            prev_points.data()[loc] = QPoint(start_x_val, \
                                            data_point_y(num));
#define DHVIS_DRAW_POINTS(color, loc, num) \
            data_point_pen.setColor(color); \
            painter->setPen(data_point_pen); \
            draw_helper(painter, total_percent, \
                        &prev_points.data()[loc], \
                        num, \
                        first);
void 
dhvis_graph_t::draw_heap_data(QPainter *painter) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
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
    foreach (dhvis_snapshot_listing_t *snapshot, *snapshots) {
        if (options->hide_peak_snapshot &&
            snapshot->is_peak) {
            continue;
        }

        total_percent += (snapshot->num_ticks / ((double)width_max)) * 100;
        if (total_percent < view_start_percent) {
            qreal start_x_val = data_point_x(total_percent - 
                                             view_start_percent);
            DHVIS_MAKE_PREV_POINTS(0, snapshot->tot_bytes_asked_for)
            DHVIS_MAKE_PREV_POINTS(1, snapshot->tot_bytes_usable)
            DHVIS_MAKE_PREV_POINTS(2, snapshot->tot_bytes_occupied)
            continue;
        } 

        if (last) {
            break;
        } else if (total_percent > view_end_percent) {
            last = true;
        }
        
        if (mem_alloc_line == true) {
            DHVIS_DRAW_POINTS(QColor(255, 102, 0), 0, 
                              &snapshot->tot_bytes_asked_for)
        }
        if (padding_line == true) {
            DHVIS_DRAW_POINTS(QColor(0, 204, 0), 1, 
                              &snapshot->tot_bytes_usable)
        }
        if (headers_line == true) {
            DHVIS_DRAW_POINTS(QColor(27, 168, 188), 2, 
                              &snapshot->tot_bytes_occupied)
        }
        first = false;

    }
    painter->restore();
}
#undef DHVIS_DRAW_POINTS
#undef DHVIS_MAKE_PREV_POINTS

/* Private
 * Draws a rectangle for a selection zoom guide
 */
void 
dhvis_graph_t::draw_selection(QPainter *painter) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    painter->save();
    /* Transparency == 40 */
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
dhvis_graph_t::mousePressEvent(QMouseEvent *event) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
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
dhvis_graph_t::mouseReleaseEvent(QMouseEvent *event) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
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
         * from drawing elongated tallies in draw_x_axis
         * (i % (int)round(diff / 4))
         */
        if (abs(view_start_percent - view_end_percent) < 2.0) {
            view_end_percent = view_start_percent + 2.0;
        }
        /* limit */
        if (view_start_percent >= 98) {
            view_end_percent = 100.0;
            view_start_percent = 98.0;
        }
        current_graph_modified = true;
        /* reset selection info */
        mouse_pressed = false;
        first_point.setX(0);
        first_point.setY(0);
        last_point.setX(0);
        last_point.setY(0);

        highlighted_snapshot();
        update();
    }
}

/* Protected
 * Interactivity for graph
 *   -Selection Zoom
 *   -Snapshot Highlighting
 */
void 
dhvis_graph_t::mouseMoveEvent(QMouseEvent *event) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
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
 * Adjusts width or height dependent variables
 */
void 
dhvis_graph_t::resizeEvent(QResizeEvent *event) 
{
    Q_UNUSED(event);
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    /* dependent on width */
    right_bound = left_bound + width() - y_axis_width() - 
                  2 * graph_outer_margin;
    highlighted_point.setX(data_point_x(highlight_percent));
    
    current_graph_modified = true;
}

/* Public slots
 * dis/en/ables line drawing
 */
void 
dhvis_graph_t::refresh_lines(int line, bool state) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
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
dhvis_graph_t::highlighted_snapshot(void) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    if(snapshots == NULL)
        return;
    qreal total_loc_lesser = view_start_percent;
    qreal total_loc_greater = view_start_percent;
    qreal total_percent = 0;
    qreal percent_diff = view_end_percent - view_start_percent;
    /* Cursor */
    highlight_percent = view_start_percent + ((double)highlighted_point.x() /
                                              (right_bound - left_bound)) *
                                             percent_diff;
    qreal highlight_loc = data_point_x(highlight_percent - view_start_percent);

    foreach (dhvis_snapshot_listing_t *snapshot, *snapshots) {
        if (options->hide_peak_snapshot &&
            snapshot->is_peak)
            continue;
        /* Snapshot locs */
        total_percent += (snapshot->num_ticks /
                         ((double)width_max)) * 100;
        total_loc_greater = data_point_x(total_percent - view_start_percent);
        /* If found */
        if (highlight_loc >= total_loc_lesser &&
            highlight_loc <= total_loc_greater) {
            if (current_snapshot_num == snapshot->snapshot_num)
                return;
            emit highlight_changed(snapshot->snapshot_num);
            current_snapshot_num = snapshot->snapshot_num;
            return;
        }
        total_loc_lesser = total_loc_greater;
    }
    return;
}

/* Public slots
 * resets the selection zoom on the graph
 */
void 
dhvis_graph_t::reset_graph_zoom(void) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    view_start_percent = 0;
    view_end_percent = 100;
    current_graph_modified = true;
    highlighted_snapshot();
    update();
}

/* Private
 * Draws the view cursor
 */
void
dhvis_graph_t::draw_view_cursor(QPainter *painter) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    painter->save();
    /* Draw cursor line */
    painter->drawLine(QPoint(highlighted_point.x(), 0),
                      QPoint(highlighted_point.x(), height()));
    /* Draw snapshot num */
    QRectF text_space(highlighted_point.x() - 3 - text_width,
                     -(height() - 1.5 * graph_outer_margin),
                     text_width,
                     text_height);
    DHVIS_DRAW_TEXT(painter->drawText(text_space,
                                      "#" + 
                                      QString::number(current_snapshot_num),
                                      QTextOption(Qt::AlignRight)))
    painter->restore();
}

/* Public Slot
 * Updates data when settings are updated
 */
void
dhvis_graph_t::update_settings(void)
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    highlighted_snapshot();
    current_graph_modified = true;
    set_heap_data(snapshots);
}

/* Public
 * Returns true if the graph's data is NULL
 */
bool
dhvis_graph_t::is_null(void) {
    return snapshots == NULL;
}