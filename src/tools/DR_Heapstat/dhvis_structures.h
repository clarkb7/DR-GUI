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

/* dhvis_structures.h
 * 
 * Defines the structures used by Dr. Heapstat
 *
 */

#ifndef DHVIS_STRUCTURES_H
#define DHVIS_STRUCTURES_H

#include <QMap>

struct dhvis_snapshot_listing_t {
    QVector<int> assoc_callstacks;
    int snapshot_num;
    qreal tot_mallocs,
          tot_bytes_asked_for,
          tot_bytes_usable,
          tot_bytes_occupied,
          num_ticks;
    bool is_peak;
};

struct dhvis_callstack_listing_t {
    QStringList frame_data;
    qreal callstack_num,
          instances, 
          bytes_asked_for,
          extra_usable,
          extra_occupied;
    QMap<int, QVector<qreal> > stale_bytes;
    QMap<int, QVector<qreal> > stale_last_access;
};

struct dhvis_options_t {
    QString def_load_dir;
    bool hide_peak_snapshot;
    int num_vertical_ticks;
    bool square_graph;
    int num_callstacks_per_page;
    bool antialiasing_enabled;
};

#endif
