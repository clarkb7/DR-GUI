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

/* dhvis_stale_graph.cpp
 * 
 * Provides the Dr. Heapstat staleness data graph
 */

#ifdef __CLASS__
#  undef __CLASS__
#endif
#define __CLASS__ "dhvis_stale_graph_t::"

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
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>

#include <cmath>

#include "dhvis_structures.h"
#include "dhvis_tool.h"
#include "dhvis_stale_graph.h"

/* Public
 * Constructor
 */
dhvis_stale_graph_t::dhvis_stale_graph_t(QVector<dhvis_callstack_listing_t *>
                                         *vec,
                                         dhvis_snapshot_listing_t *snapshot_,
                                         QString *time_unit_,
                                         dhvis_options_t *options_)
    :  callstacks(NULL), snapshot(NULL), time_unit(time_unit_), 
       graph_outer_margin(10), options(options_)
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    setAttribute(Qt::WA_DeleteOnClose);
    view_start_index = 0;
    view_end_index = 0;
    current_snapshot_num = -1;
    highlighted_point = QPoint(0, 0);
    current_graph_modified = true;
    set_heap_data(vec, snapshot_);

    /* Bottom controls */
    control_layout = new QHBoxLayout;
    view_info_label = new QLabel(tr(""), this);
    zoom_info_label = new QLabel(tr(""), this);
    reset_zoom_button = new QPushButton(tr("Reset Graph Zoom"), this);
    connect(reset_zoom_button, SIGNAL(clicked()),
            this, SLOT(reset_graph_zoom()));
    control_layout->addWidget(view_info_label);
    control_layout->addStretch(1);
    control_layout->addWidget(zoom_info_label);
    control_layout->addStretch(1);
    control_layout->addWidget(reset_zoom_button);
    view_info_label->setAlignment(Qt::AlignBottom);
    zoom_info_label->setAlignment(Qt::AlignBottom);
    control_layout->setAlignment(Qt::AlignBottom);
    setLayout(control_layout);
}

/* Private
 * Sets staleness data to be visualized
 */
void 
dhvis_stale_graph_t::set_heap_data(QVector<dhvis_callstack_listing_t *> *vec,
                                   dhvis_snapshot_listing_t *snapshot_) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    /* Memory should be taken care of by tool */
    if (vec != NULL &&
        snapshot_ != NULL) {
        callstacks = vec;        
        snapshot = snapshot_;
        current_snapshot_num = snapshot->snapshot_num;
    }
    max_height();
    max_width();
    view_end_index = width_max;

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
dhvis_stale_graph_t::paintEvent(QPaintEvent *event) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    QWidget::paintEvent(event);

    QPainter painter(this);

    /* Fix origin location */
    painter.translate(left_bound,
                      height() - 2 * reset_zoom_button->height());
    painter.scale(1, -1);

    if (callstacks == NULL || callstacks->isEmpty())
        draw_empty_graph(&painter);
    else {
        if(current_graph_modified) {
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
dhvis_stale_graph_t::max_height(void) 
{
    qreal height = 0;
    if (callstacks != NULL &&
        !callstacks->isEmpty()) {
        foreach(dhvis_callstack_listing_t *callstack, *callstacks) {
            QVector<qreal> *stale_bytes;
            stale_bytes = &(callstack->stale_bytes[current_snapshot_num]);
            foreach(qreal bytes, *stale_bytes) {
                if (bytes > height)
                    height = bytes;
            }
        }
    }
    maximum_value = QString::number(height);
    height_max = height;
}

/* Private
 * Calculates max width of x-axis
 */
void 
dhvis_stale_graph_t::max_width(void) 
{
    qreal width = 0;
    if (callstacks != NULL &&
        !callstacks->isEmpty()) {
        width = snapshot->assoc_callstacks.count();
    }
    width_max = width;
}

/* Private
 * Returns width of y_axis
 */
qreal 
dhvis_stale_graph_t::y_axis_width(void) 
{
    return text_width + 5;
}

/* Private
 * Returns height of x_axis
 */
qreal 
dhvis_stale_graph_t::x_axis_height(void) 
{
    return text_height + 5;
}

/* Private
 * Calculates x-coord for given data
 */
qreal 
dhvis_stale_graph_t::data_point_x(qreal x) 
{
    qreal max_x = width() - y_axis_width() - 2 * graph_outer_margin;
    return x * (max_x) / (double)(width_max);
}

/* Private
 * Calculates y-coord for given data
 */
qreal 
dhvis_stale_graph_t::data_point_y(qreal y) 
{
    qreal max_y = height() - x_axis_height() - 2 * reset_zoom_button->height();
    return y * (max_y) / height_max;
}

/* Private
 * Draws an empty graph when no data is present
 */
void 
dhvis_stale_graph_t::draw_empty_graph(QPainter *painter) 
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
 *  Draws the x-axis
 */
void 
dhvis_stale_graph_t::draw_x_axis(QPainter *painter) 
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
    const qreal INDEX_DIFF = view_end_index - view_start_index;
    /* Draw tallies based on num callstacks */
    for (qreal i = 0; i <= INDEX_DIFF; i++) {
        mark_width = x_axis_y - GRAPH_MARK_WIDTH;
        if ((int)i % (int)round(INDEX_DIFF / 4.0) == 0) 
            mark_width -= 2;
        painter->drawLine(QPointF(x_axis_mark, x_axis_y), 
                          QPointF(x_axis_mark, mark_width));
        /* Adjust count */
        x_axis_mark += (x_axis_width-x_axis_x) / INDEX_DIFF;
    }

    painter->restore();
}

/* Private
 * draws y-axis and scale labels
 */
void 
dhvis_stale_graph_t::draw_y_axis(QPainter *painter) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    painter->save();

    QPen y_axis_pen(QColor(qRgb(0, 0, 0)));

    qreal y_axis_x = 0;
    qreal y_axis_y = 0;
    qreal y_axis_height = height() - x_axis_height() - 
                          2 * reset_zoom_button->height();

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
                          -(y_axis_mark + text_height/2),
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
dhvis_stale_graph_t::draw_helper(QPainter *painter, qreal &next_loc, 
                                 qreal &prev_loc,
                                 dhvis_callstack_listing_t *callstack)
{
    const qreal SPACING = 2;
    QVector<qreal> &stale_bytes = callstack->stale_bytes[current_snapshot_num];
    qreal num_stales = stale_bytes.count(); 

    qreal small_area = (next_loc - prev_loc) / num_stales;
    for (qreal i = 0; i < num_stales; i++) {
        qreal dp_x = prev_loc + (small_area * i) + (SPACING / 2);
        qreal dp_y = data_point_y(stale_bytes[i]);
        painter->drawRect(QRectF(QPointF(dp_x, dp_y), 
                                 QPointF(dp_x + small_area - (SPACING / 2),
                                         0)));
    }
}

/* Private
 * Graphs data
 */
void 
dhvis_stale_graph_t::draw_heap_data(QPainter *painter) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    painter->save();

    qreal next_loc = 0;
    qreal prev_loc = 0;
    /* TODO: use preference for color*/
    QBrush data_point_brush(Qt::red);
    QPen data_point_pen(Qt::white, 3, Qt::SolidLine, 
                        Qt::RoundCap, Qt::RoundJoin);
    data_point_pen.setColor(QColor(255, 102, 0));
    if (options->antialiasing_enabled)
        painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(data_point_brush);
    painter->setPen(data_point_pen);

    const qreal INDEX_DIFF = view_end_index - view_start_index;
    int callstack_count = -1;
    foreach (int callstack_index, snapshot->assoc_callstacks) {
        callstack_count++;
        if (callstack_count < view_start_index)
            continue;
        if (callstack_count >= view_end_index)
            break;
        dhvis_callstack_listing_t *callstack;
        callstack = callstacks->at(callstack_index - 1);

        next_loc += (right_bound-left_bound) / INDEX_DIFF;
        draw_helper(painter, next_loc, prev_loc, callstack);
        prev_loc = next_loc;
    }
    painter->restore();
}

/* Private
 * Draws a rectangle for a selection zoom guide
 */
void 
dhvis_stale_graph_t::draw_selection(QPainter *painter) 
{
    painter->save();
    /* Transparency == 40 */
    QColor selection_color(0, 203, 204, 40);
    painter->setPen(QPen(selection_color));
    painter->setBrush(QBrush(selection_color, Qt::SolidPattern));
    painter->drawRect(first_point.x(), 
                      -10,
                      last_point.x() - first_point.x(),
                      last_point.y());
    painter->restore();
}

/* Protected
 * Interactivity for graph
 *   -Selection Zoom
 */
void 
dhvis_stale_graph_t::mousePressEvent(QMouseEvent *event) 
{
    if (is_null())
        return;
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
        qreal callstack_num;
        callstack_num = snapshot->assoc_callstacks
                                .at(calc_callstack_index(first_point.x()));
        zoom_info_label->setText(tr("Zoom from callstack %3 to ")
                                 .arg(callstack_num));      
    }
}

/* Protected
 * Interactivity for graph
 *   -Selection Zoom
 */
void 
dhvis_stale_graph_t::mouseReleaseEvent(QMouseEvent *event) 
{
    if (is_null())
        return;
    if (event->button() == Qt::RightButton) {
        /* Set vars to adjust graph
         * then adjust graph 
         * Use temp to avoid changing start for end's calc
         * and vice versa
         */
        qreal temp_start = calc_callstack_index(first_point.x());
        qreal temp_end = calc_callstack_index(last_point.x());
        view_start_index = temp_start;
        /* Add 1 so user doesn't have to select all of the last block */
        view_end_index = temp_end + 1;
        /* Switch if user went right to left */                    
        if (view_start_index > view_end_index) {
            qreal temp = view_start_index;
            /* Adjustment made for same reason as above */
            view_start_index = view_end_index - 1;
            view_end_index = temp + 1;
        }
        /* Floating point exception with diff < 2 
         * From drawing elongated tallies in draw_x_axis
         * (i % (int)round(diff / 4))
         */
        if (abs(view_start_index - view_end_index) < 2.0) {
            view_end_index = view_start_index + 2.0;
        }
        /* Limit */
        if (view_start_index >= width_max - 2) {
            view_end_index = width_max;
            view_start_index = width_max - 2;
        }
        current_graph_modified = true;
        
        /* Reset selection info */
        mouse_pressed = false;
        first_point.setX(0);
        first_point.setY(0);
        last_point.setX(0);
        last_point.setY(0);
        zoom_info_label->setText("");

        update();
    }
}

/* Protected
 * Interactivity for graph
 *   -Selection Zoom
 *   -Snapshot Highlighting
 */
void 
dhvis_stale_graph_t::mouseMoveEvent(QMouseEvent *event) 
{
    if (is_null())
        return;
    qreal x_val = event->pos().x();
    /* Check bounds */
    if (event->pos().x() < left_bound) {
        x_val = left_bound;
    } else if (event->pos().x() > right_bound) {
        x_val = right_bound - 1;
    }
    /* For selection zoom */
    if (event->buttons() & Qt::RightButton) {
        last_point = QPoint(x_val-left_bound,height());
        QString text = zoom_info_label->text();
        text.truncate(text.indexOf("to ") + 3);
        qreal callstack_num;
        callstack_num = snapshot->assoc_callstacks
                                .at(calc_callstack_index(last_point.x()));
        text.append(tr("%1")
                    .arg(callstack_num));
        zoom_info_label->setText(text);
    } /* For snapshot highlighting */ 
    else if (event->buttons() & Qt::LeftButton) {
        highlighted_point = event->pos();
        highlighted_point.setX(x_val-left_bound);
    }
    update();
}

/* Protected
 * Adjusts width or height dependent variables
 */
void 
dhvis_stale_graph_t::resizeEvent(QResizeEvent *event) 
{
    Q_UNUSED(event);
    /* Dependent on width */
    right_bound = left_bound + width() - y_axis_width() - 
                  2 * graph_outer_margin;
    highlighted_point.setX(data_point_x(highlight_percent));
    
    current_graph_modified = true;
}

/* Public slots
 * Resets the selection zoom on the graph
 */
void 
dhvis_stale_graph_t::reset_graph_zoom(void) 
{
    if (is_null())
        return;
    view_start_index = 0;
    view_end_index = snapshot->assoc_callstacks.count();
    current_graph_modified = true;
    update();
}

/* Private
 * Draws the view cursor
 */
void
dhvis_stale_graph_t::draw_view_cursor(QPainter *painter) 
{
    painter->save();
    /* Draw cursor line */
    painter->drawLine(QPoint(highlighted_point.x(), 0),
                      QPoint(highlighted_point.x(), height()));

    /* Get callstack index */
    const qreal INDEX_DIFF = view_end_index - view_start_index;
    qreal prec_loc = highlighted_point.x() * INDEX_DIFF /
                     (double)(right_bound-left_bound);
    qreal callstack_index = 0;
    callstack_index = calc_callstack_index(highlighted_point.x());
    qreal callstack_num = snapshot->assoc_callstacks.at(callstack_index);

    /* Put data text at bottom
     * Get stale_bytes index
     */
    qreal stale_bytes_index = 0;
    stale_bytes_index = (prec_loc - (callstack_index - view_start_index)) * 
                        callstacks->at(callstack_num - 1)
                                  ->stale_bytes[current_snapshot_num].count();
    stale_bytes_index = floor(stale_bytes_index);
    QString message;
    message = tr("%1 bytes untouched for %2 %3 in callstack #%4")
                  .arg(callstacks->at(callstack_num - 1)
                                 ->stale_bytes[current_snapshot_num]
                                              [stale_bytes_index])
                  .arg(callstacks->at(callstack_num - 1)
                                 ->stale_last_access[current_snapshot_num]
                                                    [stale_bytes_index])
                  .arg(*time_unit)
                  .arg(callstack_num);
    view_info_label->setText(message);

    painter->restore();
}

/* Public Slot
 * Updates data when settings are updated
 */
void
dhvis_stale_graph_t::update_settings(void)
{
    current_graph_modified = true;
    set_heap_data(callstacks, snapshot);
}

/* Public
 * Returns true if the graph's data is NULL
 */
bool
dhvis_stale_graph_t::is_null(void) {
    return callstacks == NULL && snapshot == NULL;
}

/* Private
 * Calculates the callstack index of the given x position
 */
qreal
dhvis_stale_graph_t::calc_callstack_index(qreal x_val) {
    if (is_null())
        return 0;
    /* Get callstack index */
    const qreal INDEX_DIFF = view_end_index - view_start_index;
    qreal callstack_index = 0;
    callstack_index = floor(x_val * INDEX_DIFF /
                            (double)(right_bound-left_bound)
                            + view_start_index);
    /* Check spaces */
    if (callstack_index >= snapshot->assoc_callstacks.count())
        callstack_index = snapshot->assoc_callstacks.count() - 1;
    else if (callstack_index < 0)
        callstack_index = 0;
    return callstack_index;
}