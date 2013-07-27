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

/* drgui_main_window.cpp
 * 
 * Provides a main structure for users to interface with tools.
 */

#ifdef __CLASS__
#  undef __CLASS__
#endif
#define __CLASS__ "drgui_main_window_t::"

#include <QtWidgets>
#include <QActionGroup>

#include "options/drgui_options_window.h"
#include "drgui_main_window.h"
#include "tools/drgui_tool_interface.h"

/* Public
 * Constructor, everything begins here
 */
drgui_main_window_t::drgui_main_window_t(void) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    window_mapper = new QSignalMapper(this);
    tab_area = new QTabWidget(this);
    tab_area->setTabsClosable(true);
    tab_area->setMovable(true);
    connect(tab_area, SIGNAL(tabCloseRequested(int)),
            this, SLOT(maybe_close(int)));
    setCentralWidget(tab_area);
    
    connect(tab_area, SIGNAL(currentChanged(int)),
            this, SLOT(update_menus()));

    create_actions();
    create_menus();
    create_status_bar();
    update_menus();
    load_tools();
    opt_win = new drgui_options_window_t(tool_action_group);

    read_settings();

    setWindowTitle(tr("DrGUI"));
    setUnifiedTitleAndToolBarOnMac(true);
}

/* Public
 * Destructor
 */
drgui_main_window_t::~drgui_main_window_t(void) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    while (plugins.count() > 0) {
        drgui_tool_interface_t *plugin = plugins.back();
        plugins.pop_back();
        delete plugin;
    }
    delete opt_win;
}

/* Protected
 * Handles closing of all tabs
 */
void 
drgui_main_window_t::closeEvent(QCloseEvent *event) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    close_all_tabs();
    if (tab_area->currentWidget() != NULL) {
        event->ignore();
    } else {
        write_settings();
        event->accept();
        /* ensure child windows close */
        qApp->quit();
    }
}

/* Private Slot
 * Shows about page for this program
 */
void 
drgui_main_window_t::about(void) 
{
   QMessageBox::about(this, tr("About DR-GUI"),
                      tr("<center><b>DR-GUI</b></center><br>"
                         "Interface for Dynamorio and various extensions"));
}

/* Slot
 * Updates the menus to reflect current tab's abilities
 */
void 
drgui_main_window_t::update_menus(void) 
{
    bool has_tool_base = (active_tool() != 0);
    close_act->setEnabled(has_tool_base);
    close_all_act->setEnabled(has_tool_base);
    next_act->setEnabled(has_tool_base);
    previous_act->setEnabled(has_tool_base);
    separator_act->setVisible(has_tool_base);
}

/* Private Slot
 * Updates the Window menu to reflect current tab's abilities
 */
void 
drgui_main_window_t::update_window_menu(void) 
{
    window_menu->clear();
    window_menu->addAction(close_act);
    window_menu->addAction(close_all_act);
    window_menu->addSeparator();
    window_menu->addAction(next_act);
    window_menu->addAction(previous_act);
    window_menu->addAction(separator_act);

    separator_act->setVisible(tab_area->currentWidget() != NULL);

    for (int i = 0; i < tab_area->count(); ++i) {
        QWidget *tool = qobject_cast<QWidget *>(tab_area->widget(i));

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(tab_area->tabText(i));
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(tab_area->tabText(i));
        }
        QAction *action  = window_menu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(tool == active_tool());
        connect(action, SIGNAL(triggered()), 
                window_mapper, SLOT(map()));
        window_mapper->setMapping(action, i);
        connect(window_mapper, SIGNAL(mapped(int)), 
                tab_area, SLOT(setCurrentIndex(int)));
    }
}

/* Private Slot
 * Creates and connects the actions for the mainwindow
 */
void 
drgui_main_window_t::create_actions(void) 
{
    separator_act = new QAction(this);
    separator_act->setSeparator(true);

    /* File */
    exit_act = new QAction(tr("E&xit"), this);
    exit_act->setShortcuts(QKeySequence::Quit);
    exit_act->setStatusTip(tr("Exit the application"));
    connect(exit_act, SIGNAL(triggered()), 
            qApp, SLOT(closeAllWindows()));

    /* Edit */
    preferences_act = new QAction(tr("&Preferences"), this);
    preferences_act->setStatusTip(tr("Edit Preferences"));
    connect(preferences_act, SIGNAL(triggered()), 
            this, SLOT(show_preferences_dialog()));

    /* Window */
    close_act = new QAction(tr("Cl&ose"), this);
    close_act->setStatusTip(tr("Close the active tab"));
    connect(close_act, SIGNAL(triggered()),
            this, SLOT(maybe_close_me()));

    close_all_act = new QAction(tr("Close &All"), this);
    close_all_act->setStatusTip(tr("Close all the tabs"));
    connect(close_all_act, SIGNAL(triggered()),
            this, SLOT(close_all_tabs()));

    next_act = new QAction(tr("Ne&xt"), this);
    next_act->setShortcuts(QKeySequence::NextChild);
    next_act->setStatusTip(tr("Move the focus to the next tab"));
    connect(next_act, SIGNAL(triggered()),
            this, SLOT(activate_next_tab()));

    previous_act = new QAction(tr("Pre&vious"), this);
    previous_act->setShortcuts(QKeySequence::PreviousChild);
    previous_act->setStatusTip(tr("Move the focus to the previous tab"));
    connect(previous_act, SIGNAL(triggered()),
            this, SLOT(activate_previous_tab()));

    /* Help */
    about_act = new QAction(tr("&About"), this);
    about_act->setStatusTip(tr("Show the application's About box"));
    connect(about_act, SIGNAL(triggered()), 
            this, SLOT(about()));

    about_qt_act = new QAction(tr("About &Qt"), this);
    about_qt_act->setStatusTip(tr("Show the Qt library's About box"));
    connect(about_qt_act, SIGNAL(triggered()), 
            qApp, SLOT(aboutQt()));

    /* Tools */
    tool_action_group = new QActionGroup(this);
}

/* Private
 * Creates the menus in the menu bar of the mainwindow
 */
void 
drgui_main_window_t::create_menus(void) 
{
    file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addSeparator();
    QAction *action = file_menu->addAction(tr("Switch layout direction"));
    connect(action, SIGNAL(triggered()), 
            this, SLOT(switch_layout_direction()));
    file_menu->addAction(exit_act);

    edit_menu = menuBar()->addMenu(tr("&Edit"));
    edit_menu->addAction(preferences_act);

    window_menu = menuBar()->addMenu(tr("&Window"));
    update_window_menu();
    connect(window_menu, SIGNAL(aboutToShow()), 
            this, SLOT(update_window_menu()));

    menuBar()->addSeparator();

    help_menu = menuBar()->addMenu(tr("&Help"));
    help_menu->addAction(about_act);
    help_menu->addAction(about_qt_act);

    tool_menu = menuBar()->addMenu(tr("&Tool"));
}

/* Private
 * Creates status bar for displaying status tips
 */
void 
drgui_main_window_t::create_status_bar(void) 
{
    statusBar()->showMessage(tr("Ready"));
}

/* Private
 * loads settings for mainwindow
 */
void 
drgui_main_window_t::read_settings(void) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    QSettings settings("Dynamorio", "DR-GUI");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

/* Private
 * Saves settings for mainwindow
 */
void 
drgui_main_window_t::write_settings(void) 
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    QSettings settings("Dynamorio", "DR-GUI");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

/* Private Slot
 * Switches direction of layout for mainwindow
 */
void 
drgui_main_window_t::switch_layout_direction(void) 
{
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

/* Private
 * finds and returns active ToolBase tab, if there is one
 */
QWidget *
drgui_main_window_t::active_tool(void) 
{
    int active_tab = tab_area->currentIndex();
    if (active_tab != -1)
        return qobject_cast<QWidget *>(tab_area->currentWidget());
    return 0;
}

/* Private Slot
 * Displays preferences dialog
 */
void 
drgui_main_window_t::show_preferences_dialog(void) 
{
    if (opt_win != NULL) {
        opt_win->display();
    }
}

/* Private Slot
 * Closes every tab in mainwindow
 */
void 
drgui_main_window_t::close_all_tabs(void) 
{
    while (tab_area->count() > 0) {
        tab_area->removeTab(0);
    }
}

/* Private Slot
 * helper for closing current tab
 */
void 
drgui_main_window_t::maybe_close_me(void) 
{
    maybe_close(tab_area->currentIndex());
}

/* Private Slot
 * Confirms closing of a tab
 */
void 
drgui_main_window_t::maybe_close(int index) 
{
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Confirm"),
                               tr("Are you sure you want to close '%1'?")
                               .arg(tab_area->tabText(index)),
                               QMessageBox::Yes | QMessageBox::No |
                               QMessageBox::Cancel);
    if (ret == QMessageBox::Yes)
        hide_tab(index);
}

/* Private Slot
 * Closes the tab at index
 */
void 
drgui_main_window_t::hide_tab(int index) 
{
    tab_area->removeTab(index);
}

/* Private Slot
 * Moves view to next tab in order, circular
 */
void 
drgui_main_window_t::activate_next_tab(void) 
{
    int index = tab_area->currentIndex() + 1;
    if (index == tab_area->count())
        index = 0;
    tab_area->setCurrentIndex(index);
}

/* Private Slot
 * Movies view to previous tab in order, loops
 */
void 
drgui_main_window_t::activate_previous_tab(void) 
{
    int index = tab_area->currentIndex() - 1;
    if (index == -1)
        index = tab_area->count() - 1;
    tab_area->setCurrentIndex(index);
}

/* Private
 * Loads available tools
 */
void 
drgui_main_window_t::load_tools(void)
{
    plugins_dir = QDir(qApp->applicationDirPath());

    #if defined(Q_OS_WIN)
        /* TODO */
    #elif defined(Q_OS_MAC)
        /* TODO */
    #endif
    plugins_dir.cd("tools");

    foreach (QString file_name, plugins_dir.entryList(QDir::Files)) {
        QPluginLoader loader(plugins_dir.absoluteFilePath(file_name), this);
        QObject *plugin = loader.instance();
        if (plugin != NULL) {
            drgui_tool_interface_t *i_tool;
            i_tool = qobject_cast<drgui_tool_interface_t *>(plugin);
            if (i_tool != NULL) {
                connect(i_tool, SIGNAL(code_editor_requested(QFile &, int)),
                        this, SLOT(create_code_editor(QFile &, int)));
                add_to_menu(plugin, i_tool->tool_names(), 
                            tool_menu, SLOT(add_tab()), tool_action_group);
                plugins.append(i_tool);
            }
            plugin_file_names += file_name;
        } else {
               QMessageBox::about(this, tr("About DR-GUI"),
                                  loader.errorString());
        }
    }
}

/* Private
 * Adds a tool to toolsMenu
 */
void 
drgui_main_window_t::add_to_menu(QObject *plugin, const QStringList &texts,
                           QMenu *menu, const char *member,
                           QActionGroup *action_group)
{
    foreach (QString text, texts) {
        QAction *action = new QAction(text, plugin);
        connect(action, SIGNAL(triggered()), 
                this, member);

        /* Hide code editor from tools menu
         * it does not need to be accesible through here
         * still needs to be handled for the options window
         */
        if (text.contains("Code Editor") != true)
            menu->addAction(action);

        if (action_group != NULL) {
            action->setCheckable(true);
            action_group->addAction(action);
        }
    }
}

/* Private Slot
 * Creates a new instance of a tool
 * and displays it in the tab interface
 */
void 
drgui_main_window_t::add_tab(void) 
{
    QAction *action = qobject_cast<QAction *>(sender());
    QWidget *tool = qobject_cast<drgui_tool_interface_t *>(action->parent())
                    ->create_instance();
    const QString tool_name = action->text();

    tab_area->setCurrentIndex(tab_area->addTab(tool, tool_name));
}

/* Private Slot
 * Handles creation of internal code_editor
 * connects to external one if requested
 */
void
drgui_main_window_t::create_code_editor(QFile &file, int line_num)
{
    qDebug().nospace() << "INFO: Entering " << __CLASS__ << __FUNCTION__;
    foreach (drgui_tool_interface_t * i_tool, plugins) {
        if (i_tool->tool_names().front().contains("Code Editor")) {
            QWidget *code_editor = i_tool->create_instance();
            i_tool->open_file(file.fileName(), line_num);
            code_editor->show();
            return;
        }
    }

    /* User must cancel or provide their own option */
    QMessageBox msg_box(this);
    msg_box.setWindowTitle(tr("Error"));
    msg_box.setText(tr("The Code Editor plugin was not found"));
    msg_box.setInformativeText(tr("Would you like to configure "
                                  "your own editor?"));
    msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No | 
                               QMessageBox::Cancel);
    msg_box.setDefaultButton(QMessageBox::Yes);
    int ret = msg_box.exec();
    /* get command from user */
    /* TODO: make list of known editor commands */
    if (ret == QMessageBox::Yes) {
        QInputDialog editor_box(this);
        editor_box.setInputMode(QInputDialog::TextInput);
        editor_box.setWindowTitle(tr("Custom Code Editor"));
        editor_box.setLabelText(tr("Launch Command Format<br> e.g. '/path/to/"
                                   "sublime-text file:line_num"));
        ret = editor_box.exec();
        QString command_format = editor_box.textValue();
        if (ret == QMessageBox::Yes) {
            custom_command_format = command_format;
        }
    }

}
