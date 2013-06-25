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

#include "tools/toolbase.h"

class QGraphicsView;
class QGraphicsScene;
class QDir;
class QFile;
class QTableWidget;
class QTextEdit;
class QPushButton;
class QLineEdit;
class QGridLayout;

struct snapshotListing {
    QVector<int> assocCallstacks;
    int snapshotNum;
    unsigned long tot_mallocs,
                  tot_bytes_asked_for,
                  tot_bytes_usable,
                  tot_bytes_occupied,
                  numTicks;
};

class DR_Heapstat_Graph;

class DR_Heapstat : public ToolBase,
                    public ToolInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.DR-GUI.ToolInterface" FILE "DR_Heapstat.json")
    Q_INTERFACES(ToolInterface)

public:
    DR_Heapstat();
    QStringList toolNames() const;
    DR_Heapstat *createInstance();

private slots:
    void loadResults();
    void fillCallstacksTable();
    void logDirTextChangedSlot();
    void loadSettings();
    void loadFramesTextEdit(int currentRow, int currentColumn, 
                            int previousRow, int previousColumn);
    void drawSnapshotGraph();
private:
    void createActions();
    void createLayout();
    bool dr_checkDir(QDir dir);
    bool dr_checkFile(QFile& file);
    void readLogData();

    /* GUI */
    QGraphicsView *graphView;
    QGraphicsScene *graphScene;

    QTableWidget *callstacksTable;
    QTextEdit *framesTextEdit;
    QPushButton *prevFrameButton;
    QPushButton *nextFrameButton;

    QLineEdit *logDirLineEdit;
    bool logDirTextChanged;
    QPushButton *loadResultsButton;

    QString logDirLoc;

    QGridLayout *leftSide;
    DR_Heapstat_Graph *snapshotGraph;

    /* Data */
    struct callstackListing {
        QStringList frameData;
        unsigned long  callstackNum,
                       instances, 
                       bytes_asked_for,
                       extra_usable,
                       extra_occupied;
    };
    QVector<struct callstackListing*> callstacks;
    QVector<struct snapshotListing*> snapshots;
};

#endif
