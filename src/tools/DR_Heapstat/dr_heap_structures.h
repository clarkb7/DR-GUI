/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: dr_heap_structures.h
** 
** Defines the structures used by Dr. Heapstat
**
*************************************************************************/

#ifndef DR_HEAP_STRUCTURES_H
#define DR_HEAP_STRUCTURES_H

struct snapshot_listing {
    QVector<int> assoc_callstacks;
    int snapshot_num;
    unsigned long tot_mallocs,
                  tot_bytes_asked_for,
                  tot_bytes_usable,
                  tot_bytes_occupied,
                  num_ticks;
};

struct callstack_listing {
    QStringList frame_data;
    unsigned long  callstack_num,
                   instances, 
                   bytes_asked_for,
                   extra_usable,
                   extra_occupied;
};

struct options_t {
    QString def_load_dir;
    bool hide_first_snapshot;
    int num_vertical_ticks;
    bool square_graph;
};

#endif
