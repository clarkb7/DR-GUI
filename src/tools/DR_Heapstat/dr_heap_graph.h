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

class QWidget;

class DR_Heapstat_Graph : public QWidget {
public:
    DR_Heapstat_Graph(QVector<struct snapshotListing*>* vec);
protected:
    void paintEvent(QPaintEvent *event);
private:
    void setHeapData(QVector<struct snapshotListing*>* vec);
    void drawEmptyGraph(QPainter *painter);
    void drawYAxis(QPainter *painter);
    void drawXAxis(QPainter *painter);
    void drawHeapData(QPainter *painter);
    unsigned long maxHeight();
    unsigned long maxWidth();
    qreal yAxisWidth();
    qreal xAxisHeight();
    qreal dataPointY(unsigned long y);
    qreal dataPointX(int x);

    QVector<struct snapshotListing*>* snapshots;
    qreal graphOuterMargin;
    QString maximumValue;
    qreal textWidth;
    qreal textHeight;
    /* TODO make max vars, func expensive*/
};

#endif
