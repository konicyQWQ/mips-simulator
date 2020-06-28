#ifndef CODELINENUMBER_H
#define CODELINENUMBER_H

#include <QWidget>
#include "codeeditor.h"

class CodeLineNumber : public QWidget
{
    Q_OBJECT
public:
    CodeLineNumber(CodeEditor *editor) : QWidget(editor), editor(editor) {

    }

    QSize sizeHint() const override {
        return QSize(LINE_NUMBER_WIDTH, 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        editor->paintLineNumber(event);
    }

private:
    CodeEditor *editor;

};

#endif // CODELINENUMBER_H
