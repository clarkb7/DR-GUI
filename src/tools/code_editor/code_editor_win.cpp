/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "code_editor_structures.h"
#include "code_editor_win.h"
#include "code_editor.h"

//! [0]
/* Public
 * Constructor
 */
code_editor_win_t::code_editor_win_t(options_t *options_)
    : options(options_)
{
    setup_menus();
    setup_editor();

    setCentralWidget(editor);
    setWindowTitle(tr("Code Editor"));

    update_settings();
}
//! [0]

/* Public Slot
 * Shows the about window
 */
void
code_editor_win_t::about(void)
{
    QMessageBox::about(this, tr("About Syntax Highlighter"),
                tr("<p>The <b>Syntax Highlighter</b> example shows how " \
                   "to perform simple syntax highlighting by subclassing " \
                   "the QSyntaxHighlighter class and describing " \
                   "highlighting rules using regular expressions.</p>"));
}

/* Public Slot
 * clears editor for new file
 */
void
code_editor_win_t::new_file(void)
{
    cur_file = QString();
    setWindowTitle(tr("Untitled"));
    editor->clear();
}

/* Public Slot
 * Opens a file for editing
 */
void
code_editor_win_t::open_file(const QString &path, int line_num)
{
    if (path == QString())
        new_file();

    QString file_name = path;

    if (file_name.isNull())
        file_name = QFileDialog::getOpenFileName(this, tr("Open File"), "", 
                                                 "C++ Files (*.cpp *.h)");

    if (!file_name.isEmpty()) {
        QFile file(file_name);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            editor->setPlainText(file.readAll());
            editor->go_to_line(line_num);
            cur_file = file_name;
            setWindowTitle(file_name);
        }
    }
}

/* Public Slot
 * Saves the current file
 */
bool
code_editor_win_t::save_as(void)
{
    QString this_file = QFileDialog::getSaveFileName(this);
    if (this_file.isEmpty())
        return false;

    return save_file(this_file);
}
/* Public Slot
 * Saves the current file
 */
bool
code_editor_win_t::save_file(const QString &file_name)
{
    /* new file */
    QString this_file = file_name;
    if (file_name.isEmpty() == true && 
        cur_file.isEmpty()== true)
        return save_as();
    /* current file */
    else if (file_name.isEmpty() == true && 
             cur_file.isEmpty() == false)
        this_file = cur_file;

    /* check valid */
    QFile file(this_file);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(this_file)
                             .arg(file.errorString()));
        return false;
    }

    /* write */
    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << editor->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    cur_file = this_file;
    setWindowTitle(this_file);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

//! [1]
/* Private
 * sets up editor options
 */
void
code_editor_win_t::setup_editor(void)
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    editor = new code_editor_t;
    editor->setFont(font);

    highlighter = new highlighter_t(editor->document());
    
    cur_file = QString();
    setWindowTitle(tr("<Untitled>"));
}
//! [1]

/* Private
 * sets up the menu-bar
 */
void
code_editor_win_t::setup_menus(void)
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);

    fileMenu->addAction(tr("&New"), this, SLOT(new_file()), QKeySequence::New);
    fileMenu->addAction(tr("&Open..."), this, SLOT(open_file()), 
                        QKeySequence::Open);
    fileMenu->addAction(tr("Save"), this, SLOT(save_file()),
                        QKeySequence::Save);
    fileMenu->addAction(tr("Save As"), this, SLOT(save_as()),
                        QKeySequence::SaveAs);
    fileMenu->addAction(tr("E&xit"), this, SLOT(close()), QKeySequence::Quit);

    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);

    helpMenu->addAction(tr("&About"), this, SLOT(about()));
    helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}

/* Public
 * updates the windows settings
 */
void
code_editor_win_t::update_settings(void)
{
    move(options->pos);
    resize(options->size);
}

/* Protected
 * closes the window and saves preferences
 */
void
code_editor_win_t::closeEvent(QCloseEvent *event) 
{
    Q_UNUSED(event);
    options->pos = pos();
    options->size = size();
    QSettings settings("Dynamorio", "DR-GUI");
    settings.beginGroup("Code Editor");
    settings.setValue("pos", options->pos);
    settings.setValue("size", options->size);
    settings.endGroup();
}