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
#include <QCheckBox>

#include <cmath>
#include <cassert>

#include "dr_heap_structures.h"
#include "dr_heap_graph.h"
#include "dr_heap_tool.h"

/* Public
 * Constructor
 */
dr_heapstat_t::dr_heapstat_t(options_t *options_) 
{
    qDebug() << "INFO: Entering dr_heapstat_t::dr_heapstat_t(void)";
    log_dir_text_changed = false;
    log_dir_loc = "";
    options = options_;
    callstacks_display_page = 0;
    current_snapshot_num = 0;
    create_actions();
    create_layout();
}

/* Public
 * Destructor
 */
dr_heapstat_t::~dr_heapstat_t(void) 
{
    qDebug() << "INFO: Entering dr_heapstat_t::~dr_heapstat_t(void)";
    for(int i = 0; i < snapshots.count();) {
        snapshot_listing *tmp = snapshots.back();
        snapshots.pop_back();
        delete tmp;
    }
    for(int i = 0; i < callstacks.count();) {
        callstack_listing *tmp = callstacks.back();
        callstacks.pop_back();
        delete tmp;
    }
    delete snapshot_graph;
}

/* Private
 * Creates and connects GUI Actions
 */
void 
dr_heapstat_t::create_actions(void) 
{
    qDebug() << "INFO: Entering dr_heapstat_t::create_actions(void)";
}

/* Private
 * Creates and connects the GUI
 */
void 
dr_heapstat_t::create_layout(void) 
{
    qDebug() << "INFO: Entering dr_heapstat_t::create_layout(void)";
    main_layout = new QGridLayout;
    /* Controls (top) */
    controls_layout = new QHBoxLayout;
    /* logdir textbox */
    log_dir_line_edit = new QLineEdit(this);
    connect(log_dir_line_edit, SIGNAL(textEdited(const QString &)), 
            this, SLOT(log_dir_text_changed_slot()));
    controls_layout->addWidget(log_dir_line_edit);
    /* load button */
    load_results_button = new QPushButton("Load Results", this);
    connect(load_results_button, SIGNAL(clicked()), 
            this, SLOT(load_results()));
    controls_layout->addWidget(load_results_button);
    controls_layout->setAlignment(load_results_button,Qt::AlignLeft);

    main_layout->addLayout(controls_layout,0,0,1,2);

    /* Left side) */
    left_side = new QGridLayout;
    /* Graph */
    graph_title = new QLabel(QString(tr("Memory consumption over "
                                        "full process lifetime")), 
                             this);
    left_side->addWidget(graph_title,0,0);
    snapshot_graph = new dr_heapstat_graph_t(NULL, NULL);
    left_side->addWidget(snapshot_graph,1,0);
    /* zoom reset button */
    reset_graph_zoom_button = new QPushButton("Reset Graph Zoom");
    left_side->addWidget(reset_graph_zoom_button,2,0);
    /* line check boxes */
    check_box_layout = new QVBoxLayout;
    mem_alloc_check_box = new QCheckBox(tr("Memory allocated ("
                                           "requested) by process"),
                                        this);
    padding_check_box = new QCheckBox(tr("Memory allocated by "
                                         "process + Padding"),
                                      this);
    headers_check_box = new QCheckBox(tr("Memory allocated by "
                                         "process + Padding "
                                         "+ Heap headers"),
                                      this);
    /* Start Checked */
    mem_alloc_check_box->setCheckState(Qt::Checked);
    padding_check_box->setCheckState(Qt::Checked);
    headers_check_box->setCheckState(Qt::Checked);
    connect(mem_alloc_check_box, SIGNAL(stateChanged(int)),
            this, SLOT(change_lines()));
    connect(padding_check_box, SIGNAL(stateChanged(int)),
            this, SLOT(change_lines()));
    connect(headers_check_box, SIGNAL(stateChanged(int)),
            this, SLOT(change_lines()));
    check_box_layout->addWidget(headers_check_box);
    check_box_layout->addWidget(padding_check_box);
    check_box_layout->addWidget(mem_alloc_check_box);
    left_side->addLayout(check_box_layout,3,0);
    /* messages box */
    messages = new QTextEdit(this);
    msg_title = new QLabel(QString(tr("Messages")), this);
    msg_layout = new QVBoxLayout;
    msg_layout->addWidget(msg_title,0);
    msg_layout->addWidget(messages,1);
    left_side->addLayout(msg_layout,4,0);
    left_side->setRowStretch(1,5);
    left_side->setRowStretch(3,2);
    left_side->setRowStretch(4,2);

    /* right side */
    right_side = new QGridLayout;
    right_title = new QLabel(QString(tr("Memory consumption at "
                                        "a given point: Individual "
                                        "callstacks")),
                             this);
    right_side->addWidget(right_title,0,0);
    /* Set up callstack table*/
    callstacks_table = new QTableWidget(this);
    connect(callstacks_table, SIGNAL(currentCellChanged(int,int,int,int)),
            this, SLOT(load_frames_text_edit(int,int,int,int)));
    right_side->addWidget(callstacks_table,1,0);
    /* Mid frame frameButtons */
    frame_buttons = new QHBoxLayout;
    prev_frame_button = new QPushButton("Prev Frames", this);
    prev_frame_button->setEnabled(false);
    connect(prev_frame_button, SIGNAL(clicked()),
            this, SLOT(show_prev_frame()));
    next_frame_button = new QPushButton("Next Frames", this);
    next_frame_button->setEnabled(false);
    connect(next_frame_button, SIGNAL(clicked()),
            this, SLOT(show_next_frame()));
    display_label = new QLabel("", this);
    frame_buttons->addWidget(prev_frame_button);
    frame_buttons->addWidget(display_label);
    frame_buttons->addStretch(1);
    frame_buttons->addWidget(next_frame_button);
    /* frame text box */
    right_side->addLayout(frame_buttons,2,0);
    frames_text_edit = new QTextEdit(this);
    frames_text_edit->setReadOnly(true);
    frames_text_edit->setLineWrapMode(QTextEdit::NoWrap);
    right_side->addWidget(frames_text_edit,3,0);
    right_side->setRowStretch(1,3);
    right_side->setRowStretch(3,5);

    main_layout->addLayout(left_side,1,0);
    main_layout->setColumnStretch(0,3);
    main_layout->addLayout(right_side,1,1);
    main_layout->setColumnStretch(1,5);
    setLayout(main_layout);
}

/* Private Slot
 * Loads log files for analysis
 */
void 
dr_heapstat_t::load_results(void) 
{
    qDebug() << "INFO: Entering dr_heapstat_t::load_results(void)";
    if (log_dir_text_changed == true) /* enter log_dir */{
        QString test_dir = log_dir_line_edit->text();
        if (dr_check_dir(QDir(test_dir)) == true) {
            log_dir_loc = test_dir;            
        } else {
            /* reset log_dir_text_changed*/
            log_dir_text_changed = false;
            return;
        }
    } else /* navigate to log_dir */ {
        QString test_dir;
        do {
        test_dir = QFileDialog::getExistingDirectory(this, 
                            tr("Open Directory"),
                            options->def_load_dir, 
                            QFileDialog::ShowDirsOnly);
        } while(dr_check_dir(QDir(log_dir_loc)) == false);
        if (test_dir.isEmpty() == true) {
            return;
        }
        log_dir_loc = test_dir;
        /* set text box text */
        log_dir_line_edit->setText(log_dir_loc);
    }
    /* reset log_dir_text_changed*/
    log_dir_text_changed = false;

    read_log_data();
    
    /* Select first callstack in table to view in frames_text_edit */
    callstacks_table->setCurrentCell(0,0);
}

/* Private
 * Reads the logs files
 */
void 
dr_heapstat_t::read_log_data(void) 
{
    qDebug() << "INFO: Entering dr_heapstat_t::read_log_data(void)";
    /* Grab and check dir */
    QDir dr_log_dir(log_dir_loc);
    if (dr_check_dir(dr_log_dir) == false) {
        return;
    }
    /* find log files */
    QFile snapshot_log(dr_log_dir.absoluteFilePath("snapshot.log"));
    QFile callstack_log(dr_log_dir.absoluteFilePath("callstack.log"));
    if (dr_check_file(callstack_log) == false ||
        dr_check_file(snapshot_log) == false) {
        return;
    }
    /* clear current callstack data*/
    callstacks_table->clear();
    callstacks_table->setRowCount(0);
    callstacks.clear();
    /* Read in callstack.log data */
    if (callstack_log.open(QFile::ReadOnly)) {
        QTextStream in_log(&callstack_log);
        QString line;
        int counter = 1;
        do /* read file */{
            do /* skip past any extra info */ {
                line = in_log.readLine();
            } while (line.contains(QString("CALLSTACK")) == false && 
                     line.contains(QString("LOG END")) == false);
            /* sanity check */
            if (line.contains(QString("LOG END")) == true) {
                break;
            }
            callstack_listing *this_callstack = new callstack_listing;
            this_callstack->callstack_num = counter;
            counter++;
            /* read in frame data */
            while(line.isNull() == false) {
                line = in_log.readLine();
                if (line.contains(QString("error end")) == true || 
                    line.isNull() == true) {
                    break;
                }
                this_callstack->frame_data << line;
    
            }
            callstacks.append(this_callstack);
        } while(line.isNull() == false && 
                line.contains(QString("LOG END")) == false);
        callstack_log.close(); 
    }
    qDebug() << "INFO: callstack.log read";

    /* Read in snapshot.log data */
    /* clear current snapshot data*/
    snapshots.clear();
    if (snapshot_log.open(QFile::ReadOnly)) {
        QTextStream in_log(&snapshot_log);
        QString line;
        int counter = 0;
        do /* read file */{
            do /* skip past any extra info */ {
                line = in_log.readLine();
            } while (line.contains(QString("SNAPSHOT #")) == false&& 
                     line.contains(QString("LOG END")) == false);
            /* sanity check */
            if (line.contains(QString("LOG END")) == true) {
                break;
            }
            snapshot_listing *this_snapshot = new snapshot_listing;
            this_snapshot->snapshot_num = counter;
            counter++;
            /* get num ticks */
            QStringList tmp_list = line.split("@").at(1).split(" ");
            foreach (QString item, tmp_list) {
                bool good_conversion = false;
                item.toULong(&good_conversion);
                if (good_conversion == true) {
                    this_snapshot->num_ticks = item.toULong();
                    break;
                }
            }
            do /* skip past any extra info */ {
                line = in_log.readLine();
            } while (line.contains(QString("total: ")) == false);
            /* separate data at commas */
            line.remove(0, 7);
            QStringList total_mem_data = line.split(",");
            this_snapshot->tot_mallocs = total_mem_data.at(0).toInt();
            this_snapshot->tot_bytes_asked_for = total_mem_data.at(1).toInt();
            this_snapshot->tot_bytes_usable = total_mem_data.at(2).toInt();
            this_snapshot->tot_bytes_occupied = total_mem_data.at(3).toInt();
            /* Add new data to callstacks */
            for( unsigned int i = 0; i < this_snapshot->tot_mallocs; ++i) {
                line = in_log.readLine();
                QStringList callstack_mem_data = line.split(",");
                callstack_listing *this_callstack; 
                this_callstack = callstacks.at(callstack_mem_data.at(0)
                                                                  .toInt()-1);

                this_callstack->instances = callstack_mem_data.at(1).toInt();
                this_callstack->bytes_asked_for = callstack_mem_data.at(2)
                                                                    .toInt();
                this_callstack->extra_usable = callstack_mem_data.at(3).toInt()
                                             + this_callstack->bytes_asked_for;
                this_callstack->extra_occupied = callstack_mem_data.at(4)
                                                                   .toInt()
                                               + this_callstack->extra_usable;
                /* ensure proper counting */
                int instance_count = this_callstack->instances;
                while(instance_count > 1) { 
                    i++;
                    instance_count--;
                }
                this_snapshot->assoc_callstacks.prepend(this_callstack->
                                                            callstack_num);
            }
            snapshots.append(this_snapshot);
        } while(line.isNull() == false && 
                line.contains(QString("LOG END")) == false);
        snapshot_log.close();
    }
    qDebug() << "INFO: snapshot.log read";
    /* Default to 0 */
    if (options->hide_first_snapshot == true)
        current_snapshot_num++;
    fill_callstacks_table(current_snapshot_num);
    draw_snapshot_graph();
}

/* Private Slot
 * Fills callstacks_table with gathered data
 */
void 
dr_heapstat_t::fill_callstacks_table(int snapshot) 
{
    qDebug() << "INFO: Entering dr_heapstat_t::fill_callstacks_table(int "
                "snapshot)";
    /* refresh settings */
    if(current_snapshot_num != snapshot) {
        callstacks_display_page = 0;
        current_snapshot_num = snapshot;
    }

    callstacks_table->clear();
    callstacks_table->setRowCount(0);
    callstacks_table->setColumnCount(5);
    QStringList table_headers;
    table_headers << tr("Call Stack") << tr("Symbol") << tr("Memory Allocated")          << tr("+Padding") << tr("+Headers");
    callstacks_table->setHorizontalHeaderLabels(table_headers);
    callstacks_table->setSortingEnabled(true);
    callstacks_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    callstacks_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    callstacks_table->setSelectionMode(QAbstractItemView::SingleSelection);
    callstacks_table->verticalHeader()->hide();
    callstacks_table->horizontalHeader()->setSectionResizeMode(
                                                QHeaderView::ResizeToContents);
    callstacks_table->horizontalHeader()->setSectionResizeMode(1, 
                                                        QHeaderView::Stretch);

    /* Put data into callstack_table */
    int row_count = -1;
    int max_rows = options->num_callstacks_per_page;
    foreach(int callstack, snapshots.at(snapshot)->assoc_callstacks) {
        row_count++;
        if (row_count < callstacks_display_page * max_rows)
            continue;
        else if (row_count >= (callstacks_display_page + 1) * max_rows)
            break;

        callstacks_table->insertRow(row_count % max_rows);
        /* Callstack number */
        QTableWidgetItem *num = new QTableWidgetItem;
        num->setData(Qt::DisplayRole, callstack);
        callstacks_table->setItem(row_count % max_rows,0,num);
        /* Symbols */
        /* Memory data */
        QTableWidgetItem *asked = new QTableWidgetItem;
        asked->setData(Qt::DisplayRole, 
                      (double)(callstacks.at(callstack-1)->bytes_asked_for));
        callstacks_table->setItem(row_count % max_rows,2,asked);

        QTableWidgetItem *padding = new QTableWidgetItem;
        padding->setData(Qt::DisplayRole, 
                        (double)(callstacks.at(callstack-1)->extra_usable));
        callstacks_table->setItem(row_count % max_rows,3,padding);

        QTableWidgetItem *headers = new QTableWidgetItem;
        headers->setData(Qt::DisplayRole, 
                        (double)(callstacks.at(callstack-1)->extra_occupied));
        callstacks_table->setItem(row_count % max_rows,4,headers);
    }
    /* Current page info */
    qreal display_num = callstacks_display_page 
                        * options->num_callstacks_per_page;
    qreal total = snapshots.at(current_snapshot_num)->assoc_callstacks.count();
    display_label->setText(tr("Displaying callstacks %1 to %2 of %3")
                       .arg(display_num + 1)
                       .arg(display_num + callstacks_table->rowCount())
                       .arg(total));
    /* Enable next button? */
    if (display_num + callstacks_table->rowCount() <  total)
        next_frame_button->setEnabled(true);
    else
        next_frame_button->setEnabled(false);
    /* Select first row */
    callstacks_table->setCurrentCell(0,0);
}

/* Private Slot
 * Returns provided tool-names during loading
 */
void 
dr_heapstat_t::log_dir_text_changed_slot(void) 
{
    qDebug() << "INFO: Entering dr_heapstat_t::log_dir_text_changed_slot"
                "(void)";
    log_dir_text_changed = true;
}

/* Private
 * Checks validity of directories
 */
bool 
dr_heapstat_t::dr_check_dir(QDir dir) 
{
    qDebug() << "INFO: Entering dr_heapstat_t::dr_check_dir(QDir dir)";
    QString error_msg = "\'"; 
    error_msg += dir.canonicalPath() 
              += "\'<br>";
    bool retVal = true;

    if (dir.exists() == false || 
        dir.isReadable() == false) {   
        qDebug() << "WARNING: Failed to open directory: " 
                 << dir.canonicalPath();
        error_msg += "is an invalid directory<br>";
        retVal = false;
    }
    if (retVal == false) {
        QMessageBox msg_box(QMessageBox::Warning, 
                            tr("Invalid Directory"),
                            error_msg, 0, this);
        msg_box.exec();
    }
    return retVal;
}

/* Private
 * Checks validity of files
 */
bool 
dr_heapstat_t::dr_check_file(QFile& file) 
{
    qDebug() << "INFO: Entering dr_heapstat_t::dr_check_file(QFile& file)";
    QString error_msg = "\'"; 
    error_msg += file.fileName() 
              += "\'<br>";
    bool retVal = true;

    if (file.exists() == false) {   
        qDebug() << "WARNING: Failed to open file: " 
                 << file.fileName();
        error_msg += "File does not exist<br>";
        retVal = false;
    } 
    if (retVal == false) {
        QMessageBox msg_box(QMessageBox::Warning, 
                            tr("Invalid File"),
                            error_msg, 0, this);
        msg_box.exec();
    }
    
    return retVal;
}

/* Private Slot
 * Loads frame data into frames_text_edit for requested callstack
 */
void 
dr_heapstat_t::load_frames_text_edit(int current_row, int current_column,
                                     int previous_row, int previous_column) 
{
    qDebug() << "INFO: Entering DR_Heapstat::load_frames_text_edit("
                "int current_row, int current_column, int previous_row, "
                "int previous_column)";
    if ( ((current_row != previous_row) ||
          (current_column != previous_column)) && 
        callstacks_table->selectedItems().size() != 0) {
        frames_text_edit->clear();
        int callstack_index = callstacks_table->item(current_row,0)
                                              ->data(Qt::DisplayRole)
                                              .toInt()-1;
        QStringList frames = callstacks.at(callstack_index)->frame_data;
        frames_text_edit->insertPlainText(QString(tr("Callstack #")));
        frames_text_edit->insertPlainText(QString::number(callstack_index+1));
        frames_text_edit->insertHtml(QString("<br>"));
        /* Add frame data */
        foreach (const QString frame, frames) {
            frames_text_edit->insertHtml(QString("<br>"));
            frames_text_edit->insertPlainText(frame);            
        }
    }
}

/* Private Slot
 * Handles creation/deletion of the graph
 */
void 
dr_heapstat_t::draw_snapshot_graph(void) 
{
    qDebug() << "INFO: Entering dr_heapstat_t::draw_snapshot_graph(void)";
    /* remove */
    left_side->removeWidget(snapshot_graph);
    delete snapshot_graph;
    /* create */
    snapshot_graph = new dr_heapstat_graph_t(&snapshots, options);  
    /* update lines */
    snapshot_graph->refresh_lines(0, mem_alloc_check_box->isChecked() == true);
    snapshot_graph->refresh_lines(1, padding_check_box->isChecked() == true);
    snapshot_graph->refresh_lines(2, headers_check_box->isChecked() == true);  

    left_side->addWidget(snapshot_graph,1,0);
    connect(snapshot_graph, SIGNAL(highlight_changed(int)),
            this, SLOT(fill_callstacks_table(int)));
    connect(reset_graph_zoom_button, SIGNAL(clicked()),
            snapshot_graph, SLOT(reset_graph_zoom()));
}

/* Private Slot
 * Sends info about which lines to draw to snapshot_graph
 */
void 
dr_heapstat_t::change_lines(void) 
{
    qDebug() << "INFO: Entering dr_heapstat_t::change_lines(void)";
    QCheckBox *emitter = (QCheckBox *)sender();
    int id = -1;
    bool state = false;
    if (emitter->text().contains(QString("Heap headers")) == true) {
        id = 2;
        state = headers_check_box->isChecked() == true;
    } else if (emitter->text().contains(QString("Padding")) == true) {
        id = 1;
        state = padding_check_box->isChecked() == true;
    } else if (emitter->text().contains(QString("requested")) == true) {
        id = 0;
        state = mem_alloc_check_box->isChecked() == true;
    }
    if (id != -1) {
        snapshot_graph->refresh_lines(id, state);
    }
}

/* Public
 * Updates widgets after a settings change
 */
void
dr_heapstat_t::update_settings(void)
{
    snapshot_graph->update_settings();
    callstacks_display_page = 0;
    fill_callstacks_table(current_snapshot_num);
}

/* Private Slot
 * Decrements page for callstacks_table
 */
void dr_heapstat_t::show_prev_frame(void) 
{
    callstacks_display_page--;
    if (callstacks_display_page == 0)
        prev_frame_button->setEnabled(false);
    fill_callstacks_table(current_snapshot_num);
}

/* Private Slot
 * Increments page for callstacks_table
 */
void dr_heapstat_t::show_next_frame(void) 
{
    callstacks_display_page++;
    prev_frame_button->setEnabled(true);
    fill_callstacks_table(current_snapshot_num);
}