/**************************************************************************
** Copyright (c) 2013, Branden Clark
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the conditions outlined in
** the COPYRIGHT file are met:
** 
** File: options_interface.h
** 
** Defines the interface for the options pages for tools
**
*************************************************************************/

#ifndef OPTIONS_INTERFACE_H
#define OPTIONS_INTERFACE_H

#include <QWidget>

class options_interface_t : public QWidget
{
    Q_OBJECT
    
public:
    virtual 
    QStringList tool_names(void) const = 0;

    virtual
    void
    write_settings(void) = 0;

};

#define OptionsInterface_iid "org.DR-GUI.OptionsInterface"

Q_DECLARE_INTERFACE(options_interface_t, OptionsInterface_iid)

#endif
