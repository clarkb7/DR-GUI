/*************************************************************************
**
** 
** File: dr_heap_tool.cpp
** 
** Defines the base of the options window.
**
**
*************************************************************************/
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringList>
#include <QPushButton>
#include <QTextEdit>
#include <QTableWidget>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QHeaderView>
#include <QTextStream>
#include <QDebug>

#include <cassert>

#include "dr_heap_tool.h"

DR_Heapstat::DR_Heapstat() {
    qDebug() << "INFO: Entering DR_Heapstat::DR_Heapstat()";
    logDirTextChanged = false;
    logDirLoc = "";
    loadSettings();
    createActions();
    createLayout();

}

void DR_Heapstat::createActions() {
    qDebug() << "INFO: Entering DR_Heapstat::createActions()";
}

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
    //controlsLayout->setAlignment(logDirLineEdit, Qt::AlignHCenter);
    /* load button */
    loadResultsButton = new QPushButton("Load Results");
    connect(loadResultsButton, SIGNAL(clicked()), 
            this, SLOT(loadResults()));
    controlsLayout->addWidget(loadResultsButton);
    controlsLayout->setAlignment(loadResultsButton,Qt::AlignLeft);



    mainLayout->addLayout(controlsLayout,0,0,1,2);

    /* Make the Graph (left side) */
    graphView = new QGraphicsView;
    graphScene = new QGraphicsScene;
    
    graphView->setBackgroundBrush(QBrush(Qt::red));
    graphView->setSceneRect(0,0,100,100);
    graphView->setScene(graphScene);
    /* y axis */
    graphScene->addLine(0,100,0,0, QPen(Qt::black));
    /* x axis */
    graphScene->addLine(0,100,100,100, QPen(Qt::black));
    graphView->setScene(graphScene);

    QGridLayout *leftSide = new QGridLayout;
    leftSide->addWidget(graphView,0,0,1,2);
    QTextEdit *messages = new QTextEdit;
    mainLayout->addWidget(messages,2,0);
    
    /* right side */
    QGridLayout *rightSide = new QGridLayout;
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
    rightSide->addWidget(callstacksTable,0,0);
    QHBoxLayout *frameButtons = new QHBoxLayout;
    prevFrameButton = new QPushButton("Prev Frames");
    nextFrameButton = new QPushButton("Next Frames");
    frameButtons->addWidget(prevFrameButton);
    frameButtons->addStretch(1);
    frameButtons->addWidget(nextFrameButton);
    rightSide->addLayout(frameButtons,1,0);
    framesTextEdit = new QTextEdit;
    rightSide->addWidget(framesTextEdit,2,0,2,1);

    mainLayout->addLayout(leftSide,1,0);
    mainLayout->addLayout(rightSide,1,1);
    
    setLayout(mainLayout);
}

void DR_Heapstat::loadResults() {
    qDebug() << "INFO: Entering DR_Heapstat::loadResults()";
    if (logDirTextChanged) {
        QString testDir = logDirLineEdit->text();
        if(dr_checkDir(QDir(testDir))) {
            logDirLoc = testDir;            
        } else {
            /* reset logDirTextChanged*/
            logDirTextChanged = false;
            return;
        }
    } else /* Select logdir */ {
        do {
        logDirLoc = QFileDialog::getExistingDirectory(this, 
                            tr("Open Directory"),
                            "/home", //FIXME: Use preference
                            QFileDialog::ShowDirsOnly
                            | QFileDialog::DontResolveSymlinks);
        } while(!dr_checkDir(QDir(logDirLoc)));
        /* set text box text */
        logDirLineEdit->setText(logDirLoc);
    }
    /* reset logDirTextChanged*/
    logDirTextChanged = false;


    readLogData();
    /* Select first callstack in table to view in framesTextEdit */
    callstacksTable->setCurrentCell(0,0);
}

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
            if(line.contains(QString("LOG END"))) {
                break;
            }
            struct callstackListing* thisCallstack = new callstackListing;
            thisCallstack->callstackNum = counter;
            counter++;
            /* read in frame data */
            while(!line.isNull()) {
                line = inLog.readLine();
                if(line.contains(QString("error end")) || line.isNull()) {
                    break;
                }
                thisCallstack->frameData << line;
    
            }
            callstacks.append(thisCallstack);
        } while(!line.isNull() && !line.contains(QString("LOG END")));
        callstackLog.close(); 
    }
    qDebug() << "INFO: callstack.log read";

    /* Read in snapshot.log data */
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
    
            for( int i = 0; i < thisSnapshot->tot_mallocs; ++i) {
                line = inLog.readLine();
                QStringList callstackMemData = line.split(",");
                struct callstackListing* thisCallstack 
                        = callstacks.at(callstackMemData.at(0).toInt()-1);
                assert(thisCallstack->callstackNum == 
                       callstackMemData.at(0).toInt());
                thisCallstack->instances = callstackMemData.at(1).toInt();
                thisCallstack->bytes_asked_for = callstackMemData.at(2)
                                                                 .toInt();
                thisCallstack->extra_usable = callstackMemData.at(3)
                                                              .toInt();
                thisCallstack->extra_occupied = callstackMemData.at(4)
                                                                .toInt();
            }
            snapshots.append(thisSnapshot);
        } while(!line.isNull() && !line.contains(QString("LOG END")));
        snapshotLog.close();
    }
    qDebug() << "INFO: snapshot.log read";

    fillCallstacksTable();
    //drawSnapshotGraph();
}

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

QStringList DR_Heapstat::toolNames() const {
    qDebug() << "INFO: Entering DR_Heapstat::toolNames()";
	return QStringList() << "DR_Heapstat";
}

DR_Heapstat *DR_Heapstat::createInstance() {
    qDebug() << "INFO: Entering DR_Heapstat::createInstance()";
    return new DR_Heapstat;
}

void DR_Heapstat::logDirTextChangedSlot() {
    qDebug() << "INFO: Entering DR_Heapstat::logDirTextChangedSlot()";
    logDirTextChanged = true;
}

void DR_Heapstat::loadSettings() {
    qDebug() << "INFO: Entering DR_Heapstat::loadSettings()";
}

bool DR_Heapstat::dr_checkDir(QDir dir) {
    qDebug() << "INFO: Entering DR_Heapstat::dr_checkDir(QDir dir)";
    if(!dir.exists() || !dir.isReadable()) {   
        QString tmpLoc(dir.canonicalPath());        
        tmpLoc.prepend('\'');
        tmpLoc.append(tr("\'<br> is an invalid directory."));
        QMessageBox msgBox(QMessageBox::Warning, 
                        tr("QMessageBox::warning()"),
                   tmpLoc, 0, this);
        msgBox.exec();
        qDebug() << "WARNING: Failed to open directory: " << tmpLoc;
        return false;
    }
    return true;
}
bool DR_Heapstat::dr_checkFile(QFile& file) {
    qDebug() << "INFO: Entering DR_Heapstat::dr_checkFile(QFile& file)";
    if(!file.exists()) {   
        QString tmpLoc(file.fileName());        
        tmpLoc.prepend('\'');
        tmpLoc.append(tr("\'<br> cannot open the file."));
        QMessageBox msgBox(QMessageBox::Warning, 
                        tr("QMessageBox::warning()"),
                   tmpLoc, 0, this);
        msgBox.exec();
        qDebug() << "WARNING: Failed to open file: " << tmpLoc;
        return false;
    }
    return true;
}

void DR_Heapstat::loadFramesTextEdit(int currentRow, int currentColumn, 
                                     int previousRow, int previousColumn) {
    qDebug() << "INFO: Entering R_Heapstat::loadFramesTextEdit("
             << "int currentRow, int currentColumn, int previousRow, "
             << "int previousColumn)";
    if (((currentRow != previousRow) ||
         (currentColumn != previousColumn)) && 
         callstacksTable->selectedItems().size() != 0) {
        framesTextEdit->clear();
        int callstackNum = callstacksTable->item(currentRow,0)
                                          ->data(Qt::DisplayRole)
                                          .toInt()-1;
        QStringList frames = callstacks.at(callstackNum)->frameData;
        foreach (const QString frame, frames) {
            framesTextEdit->insertPlainText(frame);
            framesTextEdit->insertHtml(QString("<br>"));
        }
    }
}