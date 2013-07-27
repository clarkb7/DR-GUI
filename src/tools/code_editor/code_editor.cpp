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

#include "code_editor.h"

/* Public
 * Constructor
 */
code_editor_t::code_editor_t(QWidget *parent) : QPlainTextEdit(parent)
{
    line_number_area = new line_number_area_t(this);

    connect(this, SIGNAL(blockCountChanged(int)), 
            this, SLOT(update_line_number_area_width(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), 
            this, SLOT(update_line_number_area(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), 
            this, SLOT(highlight_current_line()));

    update_line_number_area_width(0);
    highlight_current_line();
}

/* Public
 * Calculates width of the line_number_area
 */
int 
code_editor_t::line_number_area_width(void)
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

/* Private Slot
 * updates the width of the line_number_area and text_area
 */
void 
code_editor_t::update_line_number_area_width(int /* newBlockCount */)
{
    setViewportMargins(line_number_area_width(), 0, 0, 0);
}

/* Private Slot
 * updates the line_number_area
 */
void 
code_editor_t::update_line_number_area(const QRect &rect, int dy)
{
    if (dy)
        line_number_area->scroll(0, dy);
    else
        line_number_area->update(0, rect.y(), line_number_area->width(), 
                                 rect.height());

    if (rect.contains(viewport()->rect()))
        update_line_number_area_width(0);
}

/* Protected
 * updates areas according to resize
 */
void 
code_editor_t::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    line_number_area->setGeometry(QRect(cr.left(), cr.top(), 
                                  line_number_area_width(), cr.height()));
}

/* Private Slot
 * updates the width of the line_number_area
 */
void 
code_editor_t::highlight_current_line(void)
{
    QList<QTextEdit::ExtraSelection> extra_selections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor line_color = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(line_color);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extra_selections.append(selection);
    }

    setExtraSelections(extra_selections);
}

/* Public
 * draws the line_number_area
 */
void 
code_editor_t::line_number_area_paint_event(QPaintEvent *event)
{
    QPainter painter(line_number_area);
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int block_number = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset())
                                                .top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(block_number + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, line_number_area->width(), 
                             fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++block_number;
    }
}

/* Public
 * goes to a line in a file
 */
void 
code_editor_t::go_to_line(int line_num)
{
    QTextCursor text_cursor = textCursor();
    int old_line = text_cursor.blockNumber();
    int line_diff = line_num - old_line - 1;
    if (line_diff < 0)
        text_cursor.movePosition(QTextCursor::PreviousBlock,
                                 QTextCursor::MoveAnchor,
                                 -line_diff);
    else
        text_cursor.movePosition(QTextCursor::NextBlock,
                                 QTextCursor::MoveAnchor,
                                 line_diff);
    setFocus();
    setTextCursor(text_cursor);
}