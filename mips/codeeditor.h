#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include "highlightcode.h"

const int LINE_NUMBER_WIDTH = 50; // 行号的行高定义，行号的样式在codeeditor.cpp里面
const QString CODE_EDITOR_STYLE = "background: rgb(35, 40, 52);"
                                  "color: rgb(203, 204, 198);"
                                  "font: 12pt consolas;"
                                  "border: 1px solid #2d3436;"; // 样式

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    CodeEditor();
    void paintLineNumber(QPaintEvent *event);

protected:
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void updateLineNumber(const QRect &rect, int dy);

private:
    QWidget *lineNumber;
    HighlightCode *highlight;
};

#endif // CODEEDITOR_H
