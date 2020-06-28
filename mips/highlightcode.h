#ifndef HIGHLIGHTCODE_H
#define HIGHLIGHTCODE_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QColor>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>

class HighlightCode : QSyntaxHighlighter
{
    Q_OBJECT
public:
    HighlightCode(QTextDocument *document = nullptr);
    // text变化会自动调用
    void highlightBlock(const QString &text) override;
private:
    // 将满足regexp正则表达式的字符串，显示格式换为format
    void highlightIt(const QString &text, const QTextCharFormat &format, const QRegularExpression regexp);

    QTextCharFormat keywordFormat; // 关键字
    QRegularExpression keywordRegexp;
    QTextCharFormat commentFormat; // 注释
    QRegularExpression commentRegexp;
    QTextCharFormat registerFormat; // 寄存器
    QRegularExpression registerRegexp;
    QTextCharFormat digitFormat; // 数字
    QRegularExpression digitRegexp;
    QTextCharFormat stringFormat; // 字符串
    QRegularExpression stringRegexp;
    QTextCharFormat dataTypeFormat; // 格式指令 定义数据用的数据类
    QRegularExpression dataTypeRegexp;

};

#endif // HIGHLIGHTCODE_H
