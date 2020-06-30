#include "highlightcode.h"


HighlightCode::HighlightCode(QTextDocument *document) : QSyntaxHighlighter(document)
{
    // 关键字
    keywordFormat.setForeground(QColor(0xff, 0x7f, 0x50));
    keywordRegexp.setPattern("\\blb\\b|\\blh\\b|\\blw\\b|\\bsb\\b|\\bsh\\b|"
                             "\\bsw\\b|\\bmov\\b|\\bmovz\\b|\\bmovn\\b|\\btrap\\b|"
                             "\\beret\\b|\\badd\\b|\\baddu\\b|\\baddi\\b|\\baddiu\\b|"
                             "\\bsub\\b|\\bsubu\\b|\\bdiv\\b|\\bdivu\\b|\\bmul\\b|"
                             "\\bmulu\\b|\\bslt\\b|\\bslti\\b|\\bsltui\\b|\\band\\b|"
                             "\\bandi\\b|\\bor\\b|\\bori\\b|\\bxor\\b|\\bxori\\b|"
                             "\\bnor\\b|\\bnori\\b|\\bj\\b|\\bjr\\b|\\bjal\\b|"
                             "\\bjalr\\b|\\bbeq\\b|\\bbeqz\\b|\\bbenz\\b|\\bbne"
                             "\\b|\\blui\\b|\\bsll\\b|\\bsrl\\b|\\bsyscall\\b|\\bla\\b|\\bli\\b|\\binc\\b");
    // 注释
    commentFormat.setForeground(QColor(0x74, 0x7d, 0x8c));
    commentFormat.setFontItalic(true);
    commentRegexp.setPattern("#.*$");
    // 寄存器
    registerFormat.setForeground(QColor(0x70, 0xa1, 0xff));
    registerRegexp.setPattern("\\B\\$(\\d\\b|1\\d|2\\d|3[0-1]|zero|at|v[0-1]|a[0-3]|t[0-9]|s[0-7]|k[0-1]|gp|sp|fp|ra)\\b");
    // 数字
    digitFormat.setForeground(QColor(0xff, 0x6b, 0x81));
    digitRegexp.setPattern("\\b\\d*\\b");
    // 字符串
    stringFormat.setForeground(QColor(0x7b, 0xed, 0x9f));
    stringRegexp.setPattern("\".*?\"|'.*?'");
    // 格式指令 定义数据用的数据类
    dataTypeFormat.setForeground(QColor(0x82, 0xcc, 0xdd));
    dataTypeRegexp.setPattern("\\bequ\\b|\\B\\.text\\b|\\B\\.end\\b|\\B\\.data\\b|\\bdup\\b|\\B\\.zjie\\b|"
                              "\\B\\.ascii\\b|\\B\\.2zjie\\b|\\B\\.4zjie\\b|\\B\\.double\\b|\\B\\.dword\\b|\\B\\.file\\b|"
                              "\\B\\.float\\b|\\B\\.half\\b|\\B\\.space\\b|\\B\\.word\\b");
}

void HighlightCode::highlightBlock(const QString &text)
{ 
    highlightIt(text, keywordFormat, keywordRegexp);
    highlightIt(text, digitFormat, digitRegexp);
    highlightIt(text, registerFormat, registerRegexp);
    highlightIt(text, stringFormat, stringRegexp);
    highlightIt(text, dataTypeFormat, dataTypeRegexp);
    highlightIt(text, commentFormat, commentRegexp);
}

void HighlightCode::highlightIt(const QString &text, const QTextCharFormat &format, const QRegularExpression regexp)
{
    QRegularExpressionMatchIterator i = regexp.globalMatch(text);

    while(i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), format);
    }
}
