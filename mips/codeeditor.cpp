#include "codeeditor.h"
#include "codelinenumber.h"
#include "highlightcode.h"
#include <QSizePolicy>
#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QFontMetrics>

CodeEditor::CodeEditor()
{
    lineNumber = new CodeLineNumber(this);
    highlight = new HighlightCode(this->document());

    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumber);

    this->setViewportMargins(LINE_NUMBER_WIDTH, 0, 0, 0);
    this->setStyleSheet(CODE_EDITOR_STYLE);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff); // 关闭垂直滚动条

    // 设置tab长度和字体
    this->setFont(QFont("Consolas", 12));

}

void CodeEditor::paintLineNumber(QPaintEvent *event)
{
    QPainter painter(lineNumber);
    QFont font;
    QTextBlock block = firstVisibleBlock();
    int top = blockBoundingRect(block).translated(contentOffset()).top();
    int lineLimit = this->blockCount();

    // 行号的样式
    painter.setPen(QColor(211, 211, 211));
    font.setFamily("Consolas");
    font.setPointSize(11);
    painter.setFont(font);

    // 新添加的行号不会超过需要重绘的矩阵的底边界
    for(int i = block.blockNumber() + 1; top <= event->rect().bottom(); i++){
        if(i > lineLimit)
            break;
        painter.drawText(0, top, LINE_NUMBER_WIDTH, fontMetrics().height(), Qt::AlignRight, QString::number(i)+" ");
        block = block.next();
        top += fontMetrics().height();
    }

}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    // 重绘行号区域的高度
    QRect cr = contentsRect();
    lineNumber->setGeometry(cr.left(), cr.top(), LINE_NUMBER_WIDTH, cr.height());
}

void CodeEditor::updateLineNumber(const QRect &rect, int dy)
{
    if(dy)
        lineNumber->scroll(0, dy);
    else
        lineNumber->update(0, rect.y(), LINE_NUMBER_WIDTH, rect.height());
}
