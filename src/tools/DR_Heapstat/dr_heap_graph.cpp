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

#define GRAPH_MARK_WIDTH 3

#include <QWidget>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

#include "dr_heap_tool.h"
#include "dr_heap_graph.h"

/* Public
   Constructor
*/
DR_Heapstat_Graph::DR_Heapstat_Graph(QVector<struct snapshotListing*>* vec)
    : graphOuterMargin(10), maximumValue(QString::number(maxHeight())) {
    qDebug() << "INFO: Entering DR_Heapstat_Graph::DR_Heapstat_Graph"
                "(QVector<struct snapshotListing*>* vec)";
    setAttribute(Qt::WA_DeleteOnClose);
    setHeapData(vec);
}

/* Private
   Sets heap data to be visualized
*/
void DR_Heapstat_Graph::setHeapData(QVector<struct snapshotListing*>* vec) {
    qDebug() << "INFO: Entering DR_Heapstat_Graph::setHeapData"
                "(QVector<struct snapshotListing*>* vec)";
    /* memory should be taken care of by tool */
    snapshots = vec;
    QString maximumValue = QString::number(maxHeight());
    QFontMetrics fm(font());
    textHeight = fm.height();
    textWidth = fm.width(maximumValue);
    update();
}

/* Protected
   Paints an empty canvis or loads data
*/
void DR_Heapstat_Graph::paintEvent(QPaintEvent *event) {
    qDebug() << "INFO: Entering DR_Heapstat_Graph::paintEvent"
                "(QPaintEvent *event)";
    QWidget::paintEvent(event);

    QPainter painter(this);

    if (!snapshots)
        drawEmptyGraph(&painter);
    else {
        drawXAxis(&painter);
        drawYAxis(&painter);
        drawHeapData(&painter);
    }
}

/* Private
   Calculates max height of y-axis
*/
unsigned long DR_Heapstat_Graph::maxHeight() {
    qDebug() << "INFO: Entering DR_Heapstat_Graph::maxHeight()";
    unsigned long height = 0;
    if(snapshots) {
        foreach(struct snapshotListing* snapshot, *snapshots) {
            if (snapshot->tot_bytes_occupied > height)
                height = snapshot->tot_bytes_occupied;
        }
    }
    return height;
}

/* Private
   Calculates max width of x-axis
*/
unsigned long DR_Heapstat_Graph::maxWidth() {
    qDebug() << "INFO: Entering DR_Heapstat_Graph::maxWidth()";
    unsigned long width = 0;

    foreach(struct snapshotListing* snapshot, *snapshots) {
        width += snapshot->numTicks;
    }

    return width;
}

/* Private
   Returns width of yAxis
*/
qreal DR_Heapstat_Graph::yAxisWidth() {
    qDebug() << "INFO: Entering DR_Heapstat_Graph::yAxisWidth()";
    return textWidth + 5;
}

/* Private
   Returns height of xAxis
*/
/* XXX: no text on x-axis, probably don't need */
qreal DR_Heapstat_Graph::xAxisHeight() {
    qDebug() << "INFO: Entering DR_Heapstat_Graph::xAxisWidth()";
    return textHeight + 5;
}

/* Private
   Calculates x-coord for given data
*/
qreal DR_Heapstat_Graph::dataPointX(int x) {
    qDebug() << "INFO: Entering DR_Heapstat_Graph::dataPointX(int x)";
    int maxX = width() - yAxisWidth() - 2*graphOuterMargin;
    return x * (maxX) / snapshots->count() ;
}

/* Private
   Calculates y-coord for given data
*/
qreal DR_Heapstat_Graph::dataPointY(unsigned long y) {
    qDebug() << "INFO: Entering DR_Heapstat_Graph::dataPointY"
                "(unsigned long y)";
    int maxY = height() - xAxisHeight() - 2*graphOuterMargin;
    return y * (maxY) / maxHeight();
}

/* Private
   Draws an empty graph when no data is present
*/
void DR_Heapstat_Graph::drawEmptyGraph(QPainter *painter) {
    qDebug() << "INFO: Entering DR_Heapstat_Graph::drawEmptyGraph"
                "(QPainter *painter)";
    QString msg("No datapoints available!");

    qreal centreX = width() / 2;
    qreal centreY = height() / 2;

    QFontMetricsF fontMetrics(font());
    qreal msgWidth = fontMetrics.width(msg);
    qreal msgHeight = fontMetrics.height();

    qreal msgX = centreX - (msgWidth / 2);
    qreal msgY = centreY - (msgHeight / 2);

    painter->drawText(QPointF(msgX, msgY), msg);
}

/* Private
   Draws the x-axis
*/
void DR_Heapstat_Graph::drawXAxis(QPainter *painter) {
   qDebug() << "INFO: Entering DR_Heapstat_Graph::drawXAxis"
               "(QPainter *painter)";
    painter->save();
    /* Fix origin location */
    painter->translate(graphOuterMargin + textWidth + 5,
                       height() - graphOuterMargin);
    painter->scale(1,-1);

    QPen xAxisPen(QColor(qRgb(0, 0, 0)));

    qreal xAxisX = 0;
    qreal xAxisY = 0;
    qreal xAxisWidth = width() - yAxisWidth() - 2*graphOuterMargin;

    painter->setPen(xAxisPen);
    painter->drawLine(QPointF(xAxisX, xAxisY), 
                      QPointF(xAxisWidth, xAxisY));

    //TODO: Use preference (whole thing)
    //TODO: not just ticks
    qreal xAxisMark = xAxisX;
    foreach(struct snapshotListing* snapshot, *snapshots) {
        Q_UNUSED(snapshot);
        painter->drawLine(QPointF(xAxisMark, xAxisY), 
                          QPointF(xAxisMark, xAxisY - GRAPH_MARK_WIDTH));
        xAxisMark += (xAxisWidth-xAxisX) / snapshots->count();
    }
    /* draw max separately to avoid rounding errors */
    painter->drawLine(QPointF(xAxisWidth, xAxisY - GRAPH_MARK_WIDTH), 
                      QPointF(xAxisWidth, xAxisY));

    painter->restore();
}

/* Private
   draws y-axis and scale labels
*/
void DR_Heapstat_Graph::drawYAxis(QPainter *painter) {
    qDebug() << "INFO: Entering DR_Heapstat_Graph::drawYAxis"
                "(QPainter *painter)";
    painter->save();
    /* Fix origin location */
    painter->translate(graphOuterMargin + textWidth + 5,
                       height() - graphOuterMargin);
    painter->scale(1,-1);

    QPen yAxisPen(QColor(qRgb(0, 0, 0)));

    qreal yAxisX = 0;
    qreal yAxisY = 0;
    qreal yAxisHeight = height() - yAxisY 
                        - xAxisHeight() - 2*graphOuterMargin;

    painter->setPen(yAxisPen);
    painter->drawLine(QPointF(yAxisX, yAxisY), 
                      QPointF(yAxisX, yAxisHeight));

    /* Draw scale */
    qreal yAxisMark = 0;
    long curValue = 0;
    long maxVal = maximumValue.toLong();
    int numTabs = 9; //TODO: Use preference
    for (int count = 0; count < numTabs;++count) {
        /* fix scale for text, it appears upside down */
        painter->save();
        painter->scale(1,-1);
        painter->drawText(QRectF(-(textWidth + graphOuterMargin), 
                                 -(yAxisMark + textHeight/2),
                                 textWidth,
                                 textHeight),
                          QString::number(curValue),
                          QTextOption(Qt::AlignRight));
        painter->restore();
        painter->drawLine(QPointF(yAxisX - GRAPH_MARK_WIDTH, yAxisMark), 
                          QPointF(yAxisX, yAxisMark));
        yAxisMark += (yAxisHeight-yAxisY) / numTabs;
        curValue += maxVal / numTabs;
    }
    /* draw max separately to avoid rounding errors */
    painter->drawLine(QPointF(yAxisX - GRAPH_MARK_WIDTH, yAxisHeight), 
                      QPointF(yAxisX, yAxisHeight));
    /* fix scale for text, it appears upside down */
    painter->save();
    painter->scale(1,-1);
    painter->drawText(QRectF(-(textWidth + graphOuterMargin), 
                             -(yAxisHeight + textHeight/2),
                             textWidth,
                             textHeight),
                      maximumValue,
                      QTextOption(Qt::AlignRight));
    painter->restore();
    painter->restore();
}

/* Private
   Graphs data
*/
void DR_Heapstat_Graph::drawHeapData(QPainter *painter) {
    qDebug() << "INFO: Entering DR_Heapstat_Graph::drawHeapData"
                "(QPainter *painter)";
    painter->save();
    /* Fix origin location */
    painter->translate(graphOuterMargin + textWidth + 5,
                       height() - graphOuterMargin);
    painter->scale(1,-1);

    QPoint lastPoint(dataPointX(snapshots->at(0)->snapshotNum),
                     dataPointY(snapshots->at(0)->tot_bytes_occupied));

    foreach(struct snapshotListing* snapshot, *snapshots) {
        /* TODO: use preference for color*/
        QBrush dataPointBrush(Qt::red);
        QPen dataPointPen(QColor(qRgb(0, 0, 0)));

        painter->save();

        painter->setBrush(dataPointBrush);
        painter->setPen(dataPointPen);

        qreal dpX = dataPointX(snapshot->snapshotNum);
        qreal dpY = dataPointY(snapshot->tot_bytes_occupied);
        
        QPoint thisPoint(dpX, dpY);
        painter->drawLine(lastPoint, thisPoint);
        painter->drawRect(dpX,dpY,5,5);
        lastPoint.setX(dpX);
        lastPoint.setY(dpY);
        painter->restore();
    }
    painter->restore();
}