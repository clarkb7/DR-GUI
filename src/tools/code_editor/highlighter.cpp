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

#include "highlighter.h"

//! [0]
/* Public
 * Constructor
 */
highlighter_t::highlighter_t(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    highlighting_rule_t rule;

    keyword_format.setForeground(Qt::darkBlue);
    keyword_format.setFontWeight(QFont::Bold);
    QStringList keyword_patterns;
    keyword_patterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                     << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                     << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                     << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                     << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                     << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                     << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                     << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                     << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                     << "\\bvoid\\b" << "\\bvolatile\\b";
    foreach (const QString &pattern, keyword_patterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keyword_format;
        highlighting_rules.append(rule);
//! [0] //! [1]
    }
//! [1]

//! [2]
    class_format.setFontWeight(QFont::Bold);
    class_format.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = class_format;
    highlighting_rules.append(rule);
//! [2]

//! [3]
    single_line_comment_format.setForeground(Qt::red);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = single_line_comment_format;
    highlighting_rules.append(rule);

    multi_line_comment_format.setForeground(Qt::red);
//! [3]

//! [4]
    quotation_format.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotation_format;
    highlighting_rules.append(rule);
//! [4]

//! [5]
    function_format.setFontItalic(true);
    function_format.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = function_format;
    highlighting_rules.append(rule);
//! [5]

//! [6]
    comment_start_expression = QRegExp("/\\*");
    comment_end_expression = QRegExp("\\*/");
}
//! [6]

//! [7]
/* Protected
 * applies syntax highlighting rules
 */
void 
highlighter_t::highlightBlock(const QString &text)
{
    foreach (const highlighting_rule_t &rule, highlighting_rules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
//! [7] //! [8]
    setCurrentBlockState(0);
//! [8]

//! [9]
    int start_index = 0;
    if (previousBlockState() != 1)
        start_index = comment_start_expression.indexIn(text);

//! [9] //! [10]
    while (start_index >= 0) {
//! [10] //! [11]
        int end_index = comment_end_expression.indexIn(text, start_index);
        int comment_length;
        if (end_index == -1) {
            setCurrentBlockState(1);
            comment_length = text.length() - start_index;
        } else {
            comment_length = end_index - start_index
                             + comment_end_expression.matchedLength();
        }
        setFormat(start_index, comment_length, multi_line_comment_format);
        start_index = comment_start_expression.indexIn(text, 
                                                      start_index + 
                                                      comment_length);
    }
}
//! [11]
