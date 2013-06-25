/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: dr_heap_tool.cpp
** 
** Provides the DR. Heapstat tool
**
*************************************************************************/

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QTableWidget>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QHeaderView>
#include <QDebug>

#include <cmath>
#include <cassert>

#include "dr_heap_tool.h"
#include "dr_heap_graph.h"

/* Public
   Constructor
*/
DR_Heapstat::DR_Heapstat() {
    qDebug() << "INFO: Entering DR_Heapstat::DR_Heapstat()";
    logDirTextChanged = false;
    logDirLoc = "";
    loadSettings();
    createActions();
    createLayout();

}

/* Private
   Creates and connects GUI Actions
*/
void DR_Heapstat::createActions() {
    qDebug() << "INFO: Entering DR_Heapstat::createActions()";
}

/* Private
   Creates and connects the GUI
*/
void DR_Heapstat::createLayout() {
    qDebug() << "INFO: Entering DR_Heapstat::createLayout()";
    QGridLayout *mainLayout = new QGridLayout;
    /* Controls (top) */
    QHBoxLayout *controlsLayout = new QHBoxLayout;
    /* logdir textbox */
    logDirLineEdit = new QLineEdit();
    connect(logDirLineEdit, SIGNAL(textEdited(const QString &)), 
            this, SLOT(logDirTextChangedSlot()));
    controlsLayout->addWidget(logDirLineEdit);
    /* load button */
    loadResultsButton = new QPushButton("Load Results");
    connect(loadResultsButton, SIGNAL(clicked()), 
            this, SLOT(loadResults()));
    controlsLayout->addWidget(loadResultsButton);
    controlsLayout->setAlignment(loadResultsButton,Qt::AlignLeft);

    mainLayout->addLayout(controlsLayout,0,0,1,2);

    /* Left side) */
    leftSide = new QGridLayout;
    /* Graph */
    QLabel*graphTitle = new QLabel(QString(tr("Memory consumption over "
                                "full process lifetime")));
    leftSide->addWidget(graphTitle,0,0);
    snapshotGraph = new DR_Heapstat_Graph(NULL);
    leftSide->addWidget(snapshotGraph,1,0);
    /* messages box */
    QTextEdit *messages = new QTextEdit;
    QLabel *msgTitle = new QLabel(QString(tr("Messages")));
    QVBoxLayout *msgLayout = new QVBoxLayout;
    msgLayout->addWidget(msgTitle,0);
    msgLayout->addWidget(messages,1);
    leftSide->addLayout(msgLayout,2,0);
    leftSide->setRowStretch(1,5);
    leftSide->setRowStretch(2,2);
    
    /* right side */
    QGridLayout *rightSide = new QGridLayout;
    QLabel *rightTitle = new QLabel(QString(tr("Memory consumption at "
                             "a given point: Individual callstacks")));
    rightSide->addWidget(rightTitle,0,0);
    /* Set up callstack table*/
    callstacksTable = new QTableWidget;
    callstacksTable->setColumnCount(5);
    QStringList tableHeaders;
    tableHeaders << tr("Call Stack") << tr("Symbol") 
                 << tr("Memory Allocated") << tr("+Padding") 
                 << tr("+Headers");
    callstacksTable->setHorizontalHeaderLabels(tableHeaders);
    callstacksTable->setSortingEnabled(true);
    callstacksTable->horizontalHeader()->setStretchLastSection(true);
    callstacksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    callstacksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    callstacksTable->setSelectionMode(QAbstractItemView::SingleSelection);
    callstacksTable->verticalHeader()->hide();
    connect(callstacksTable, SIGNAL(currentCellChanged(int,int,int,int)),
            this, SLOT(loadFramesTextEdit(int,int,int,int)));
    rightSide->addWidget(callstacksTable,1,0);
    /* Mid frame frameButtons */
    QHBoxLayout *frameButtons = new QHBoxLayout;
    prevFrameButton = new QPushButton("Prev Frames");
    nextFrameButton = new QPushButton("Next Frames");
    frameButtons->addWidget(prevFrameButton);
    frameButtons->addStretch(1);
    frameButtons->addWidget(nextFrameButton);
    /* frame text box */
    rightSide->addLayout(frameButtons,3,0);
    framesTextEdit = new QTextEdit;
    rightSide->addWidget(framesTextEdit,4,0);
    rightSide->setRowStretch(4,3);

    mainLayout->addLayout(leftSide,1,0);
    mainLayout->setColumnStretch(0,3);
    mainLayout->addLayout(rightSide,1,1);
    mainLayout->setColumnStretch(1,5);
    setLayout(mainLayout);
}

/* Private Slot
   Loads log files for analysis
*/
void DR_Heapstat::loadResults() {
    qDebug() << "INFO: Entering DR_Heapstat::loadResults()";
    if (logDirTextChanged) /* enter logDir */{
        QString testDir = logDirLineEdit->text();
        if(dr_checkDir(QDir(testDir))) {
            logDirLoc = testDir;            
        } else {
            /* reset logDirTextChanged*/
            logDirTextChanged = false;
            return;
        }
    } else /* navigate to logDir */ {
        QString testDir;
        do {
        testDir = QFileDialog::getExistingDirectory(this, 
                            tr("Open Directory"),
                            "/home", //TODO: Use preference
                            QFileDialog::ShowDirsOnly);
        } while(!dr_checkDir(QDir(logDirLoc)));
        if(testDir.isEmpty()) {
            return;
        }
        logDirLoc = testDir;
        /* set text box text */
        logDirLineEdit->setText(logDirLoc);
    }
    /* reset logDirTextChanged*/
    logDirTextChanged = false;

    readLogData();
    
    /* Select first callstack in table to view in framesTextEdit */
    callstacksTable->setCurrentCell(0,0);
}

/* Private
   Reads the logs files
*/
void DR_Heapstat::readLogData() {
    qDebug() << "INFO: Entering DR_Heapstat::readLogData()";
    /* Grab and check dir */
    QDir dr_logDir(logDirLoc);
    if(!dr_checkDir(dr_logDir)) {
        return;
    }
    /* find log files */
    QFile snapshotLog(dr_logDir.absoluteFilePath("snapshot.log"));
    QFile callstackLog(dr_logDir.absoluteFilePath("callstack.log"));
    if(!dr_checkFile(callstackLog) ||
       !dr_checkFile(snapshotLog)) {
        return;
    }
    /* clear current callstack data*/
    callstacksTable->clearContents();
    callstacksTable->setRowCount(0);
    while(callstacks.count() != 0) {
        struct callstackListing* tmp = callstacks.back();
        callstacks.pop_back();
        delete tmp;
    }
    /* Read in callstack.log data */
    if(callstackLog.open(QFile::ReadOnly)) {
        QTextStream inLog(&callstackLog);
        QString line;
        int counter = 1;
        do /* read file */{
            do /* skip past any extra info */ {
                line = inLog.readLine();
            } while (!line.contains(QString("CALLSTACK")) && 
                     !line.contains(QString("LOG END")));
            /* sanity check */
            if(line.contains(QString("LOG END"))) {
                break;
            }
            struct callstackListing* thisCallstack = new callstackListing;
            thisCallstack->callstackNum = counter;
            counter++;
            /* read in frame data */
            while(!line.isNull()) {
                line = inLog.readLine();
                if(line.contains(QString("error end")) || 
                   line.isNull()) {
                    break;
                }
                thisCallstack->frameData << line;
    
            }
            callstacks.append(thisCallstack);
        } while(!line.isNull() && 
                !line.contains(QString("LOG END")));
        callstackLog.close(); 
    }
    qDebug() << "INFO: callstack.log read";

    /* Read in snapshot.log data */
    /* TODO: Make preference to ignore first snapshot
              It's usually MUCH larger than the rest of the life cycle
              Won't be necessary when start and finish life-time
              view sliders are implemented
    */
    if(snapshotLog.open(QFile::ReadOnly)) {
        QTextStream inLog(&snapshotLog);
        QString line;
        int counter = 0;
        do /* read file */{
            do /* skip past any extra info */ {
                line = inLog.readLine();
            } while (!line.contains(QString("SNAPSHOT #")) && 
                     !line.contains(QString("LOG END")));
            if(line.contains(QString("LOG END"))) {
                break;
            }
            struct snapshotListing* thisSnapshot = new snapshotListing;
            thisSnapshot->snapshotNum = counter;
            counter++;
            /* get num ticks */
            QStringList tmpList = line.split("@").at(1).split(" ");
            foreach (QString item, tmpList) {
                bool goodConversion;
                item.toInt(&goodConversion);
                if(goodConversion) {
                    thisSnapshot->numTicks = item.toULong();
                    break;
                }
            }
            do /* skip past any extra info */ {
                line = inLog.readLine();
            } while (!line.contains(QString("total: ")));
            /* separate data at commas */
            line.remove(0, 7);
            QStringList totalMemData = line.split(",");
            thisSnapshot->tot_mallocs = totalMemData.at(0).toInt();
            thisSnapshot->tot_bytes_asked_for = totalMemData.at(1).toInt();
            thisSnapshot->tot_bytes_usable = totalMemData.at(2).toInt();
            thisSnapshot->tot_bytes_occupied = totalMemData.at(3).toInt();
            /* Add new data to callstacks */
            for( unsigned int i = 0; i < thisSnapshot->tot_mallocs; ++i) {
                line = inLog.readLine();
                QStringList callstackMemData = line.split(",");
                struct callstackListing* thisCallstack 
                        = callstacks.at(callstackMemData.at(0).toInt()-1);
                thisCallstack->instances = callstackMemData.at(1).toInt();
                thisCallstack->bytes_asked_for = callstackMemData.at(2)
                                                                 .toInt();
                thisCallstack->extra_usable = callstackMemData.at(3)
                                                              .toInt();
                thisCallstack->extra_occupied = callstackMemData.at(4)
                                                                .toInt();
                /* ensure proper counting */
                int instanceCount = thisCallstack->instances;
                while(instanceCount > 1) { 
                    i++;
                    instanceCount--;
                }
            }
            snapshots.append(thisSnapshot);
        } while(!line.isNull() && 
                !line.contains(QString("LOG END")));
        snapshotLog.close();
    }
    qDebug() << "INFO: snapshot.log read";

    fillCallstacksTable();
    drawSnapshotGraph();
}

/* Private Slot
   Fills callstacksTable with gathered data
*/
/* TODO fill in more data, have it be specific to snapshot*/
void DR_Heapstat::fillCallstacksTable() {
    qDebug() << "INFO: Entering DR_Heapstat::fillCallstackTable()";

    /* Put data into callstackTable */
    for(int i=0; i < callstacks.size(); i++) {
        callstacksTable->insertRow(i);
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setData(Qt::DisplayRole, i+1);
        callstacksTable->setItem(i,0,item);
    }
}

/* Public
   Returns provided tool-names during loading
*/
QStringList DR_Heapstat::toolNames() const {
    qDebug() << "INFO: Entering DR_Heapstat::toolNames()";
    return QStringList() << "DR_Heapstat";
}

/* Public
   Returns a new instance of the tool
*/
DR_Heapstat *DR_Heapstat::createInstance() {
    qDebug() << "INFO: Entering DR_Heapstat::createInstance()";
    return new DR_Heapstat;
}

/* Private Slot
   Returns provided tool-names during loading
*/
void DR_Heapstat::logDirTextChangedSlot() {
    qDebug() << "INFO: Entering DR_Heapstat::logDirTextChangedSlot()";
    logDirTextChanged = true;
}

/* Private Slot
   Loads preferences data
*/
void DR_Heapstat::loadSettings() {
    qDebug() << "INFO: Entering DR_Heapstat::loadSettings()";
}

/* Private
   Checks validity of directories
*/
bool DR_Heapstat::dr_checkDir(QDir dir) {
    qDebug() << "INFO: Entering DR_Heapstat::dr_checkDir(QDir dir)";
    QString errorMsg = "\'"; errorMsg += dir.canonicalPath() += "\'<br>";
    bool retVal = true;

    if(!dir.exists() || !dir.isReadable()) {   
        qDebug() << "WARNING: Failed to open directory: " 
                 << dir.canonicalPath();
        errorMsg += "is an invalid directory<br>";
        retVal = false;
    }
    if(!retVal) {
        QMessageBox msgBox(QMessageBox::Warning, 
                           tr("Invalid Directory"),
                           errorMsg, 0, this);
        msgBox.exec();
    }
    return retVal;
}

/* Private
   Checks validity of directories
*/
bool DR_Heapstat::dr_checkFile(QFile& file) {
    qDebug() << "INFO: Entering DR_Heapstat::dr_checkFile(QFile& file)";
    QString errorMsg = "\'"; errorMsg += file.fileName() += "\'<br>";
    bool retVal = true;

    if(!file.exists()) {   
        qDebug() << "WARNING: Failed to open file: " 
                 << file.fileName();
        errorMsg += "File does not exist<br>";
        retVal = false;
    } 
    if(!retVal) {
        QMessageBox msgBox(QMessageBox::Warning, 
                           tr("Invalid File"),
                           errorMsg, 0, this);
        msgBox.exec();
    }
    
    return retVal;
}

/* Private Slot
   Loads frame data into framesTextEdit for requested callstack
*/
void DR_Heapstat::loadFramesTextEdit(int currentRow, int currentColumn, 
                                     int previousRow, int previousColumn) {
    qDebug() << "INFO: Entering R_Heapstat::loadFramesTextEdit("
                "int currentRow, int currentColumn, int previousRow, "
                "int previousColumn)";
    if (((currentRow != previousRow) ||
         (currentColumn != previousColumn)) && 
         callstacksTable->selectedItems().size() != 0) {
        framesTextEdit->clear();
        int callstackIndex = callstacksTable->item(currentRow,0)
                                            ->data(Qt::DisplayRole)
                                            .toInt()-1;
        QStringList frames = callstacks.at(callstackIndex)->frameData;
        framesTextEdit->insertPlainText(QString(tr("Callstack #")));
        framesTextEdit->insertPlainText(QString::number(callstackIndex+1));
        framesTextEdit->insertHtml(QString("<br>"));
        /* Add frame data */
        foreach (const QString frame, frames) {
            framesTextEdit->insertHtml(QString("<br>"));
            framesTextEdit->insertPlainText(frame);            
        }
    }
}

/* Private Slot
   Handles creation/deletion of the graph
*/
void DR_Heapstat::drawSnapshotGraph() {
    qDebug() << "INFO: Entering DR_Heapstat::drawSnapshotGraph()";
    leftSide->removeWidget(snapshotGraph);
    delete snapshotGraph;
    snapshotGraph = new DR_Heapstat_Graph(&snapshots);
    leftSide->addWidget(snapshotGraph,1,0);
}
