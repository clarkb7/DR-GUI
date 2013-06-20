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
#include "dr_heap_tool.h"

DR_Heapstat::DR_Heapstat() {
    graphView = new QGraphicsView;
    graphScene = new QGraphicsScene;

    /* Make the Graph */
    graphView->setBackgroundBrush(QBrush(Qt::red));
    graphView->setSceneRect(0,0,100,100);
    graphView->setScene(graphScene);
    /* y axis */
    graphScene->addLine(0,100,0,0, QPen(Qt::black));
    /* x axis */
    graphScene->addLine(0,100,100,100, QPen(Qt::black));

    graphView->setScene(graphScene);
    QGridLayout *mainLayout = new QGridLayout;
    QGridLayout *leftSide = new QGridLayout;
    leftSide->addWidget(graphView,0,0,1,2);

    QTextEdit *messages = new QTextEdit;
    mainLayout->addWidget(messages,2,0);
    
    QGridLayout *rightSide = new QGridLayout;
    QTableWidget *framesTable = new QTableWidget;
    framesTable->setColumnCount(20);
    framesTable->setRowCount(20);
    rightSide->addWidget(framesTable,0,0);
    QHBoxLayout *frameButtons = new QHBoxLayout;
    QPushButton *prevFrameButton = new QPushButton;
    QPushButton *nextFrameButton = new QPushButton;
    frameButtons->addWidget(prevFrameButton);
    frameButtons->addWidget(nextFrameButton);
    rightSide->addLayout(frameButtons,1,0);
    QTextEdit *callstacks = new QTextEdit;
    rightSide->addWidget(callstacks,2,0,2,1);

    mainLayout->addLayout(leftSide,0,0);
	mainLayout->addLayout(rightSide,0,1);
    
    
    setLayout(mainLayout);
}

QStringList DR_Heapstat::toolNames() const {
	return QStringList() << "DR_Heapstat";
}

DR_Heapstat *DR_Heapstat::createInstance() {
    return new DR_Heapstat;
}