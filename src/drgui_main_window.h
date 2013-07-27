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

/* drgui_main_window.h
 * 
 * Defines a main structure for users to interface with tools.
 */

#ifndef DRGUI_MAIN_WINDOW_H
#define DRGUI_MAIN_WINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QStringList>

class OptionBase;
class tool_base_t;
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QActionGroup;
class QPluginLoader;

class drgui_options_window_t;
class drgui_tool_interface_t;

class drgui_main_window_t : public QMainWindow
{
    Q_OBJECT

public:
    drgui_main_window_t(void);
    ~drgui_main_window_t(void);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void 
    about(void);

    void 
    update_menus(void);

    void 
    update_window_menu(void);

    void 
    switch_layout_direction(void);

    void 
    show_preferences_dialog(void);
    
    void 
    maybe_close_me(void);
    
    void 
    maybe_close(int index);
    
    void 
    add_tab(void);
    
    void 
    hide_tab(int index);
    
    void 
    close_all_tabs(void);
    
    void 
    activate_next_tab(void);
    
    void 
    activate_previous_tab(void);

    void
    create_code_editor(QFile &file, int line_num);
    
private:
    void 
    create_actions(void);
    
    void 
    create_menus(void);
    
    void 
    create_status_bar(void);
    
    void 
    read_settings(void);
    
    void 
    write_settings(void);
    
    QWidget *
    active_tool(void);
    
    void 
    load_tools(void);
    
    void 
    add_to_menu(QObject *plugin, const QStringList &texts,
                QMenu *menu, const char *member,
                QActionGroup *action_group);

    /* GUI */
    QDir plugins_dir;
    QStringList plugin_file_names;
    QTabWidget *tab_area;
    QSignalMapper *window_mapper;
    QActionGroup *tool_action_group;
    QAction *separator_act;

    QMenu *file_menu;
    /*Switch layout act */
    QAction *exit_act;

    QMenu *edit_menu;
    QAction *preferences_act;
    drgui_options_window_t *opt_win;

    QMenu *window_menu;
    QAction *close_act;
    QAction *close_all_act;
    QAction *next_act;
    QAction *previous_act;

    QMenu *help_menu;    
    QAction *about_act;
    QAction *about_qt_act;

    QMenu *tool_menu;

    QVector<drgui_tool_interface_t *> plugins;

    /* options */
    QString custom_command_format;
};

#endif
