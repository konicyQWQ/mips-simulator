#ifndef MIPS_H
#define MIPS_H

#include <QWidget>
#include <QFile>
#include "codeeditor.h"
#include "MipsCPU.h"

QT_BEGIN_NAMESPACE
namespace Ui { class mips; }
QT_END_NAMESPACE

class mips : public QWidget
{
    Q_OBJECT

public:
    mips(QWidget *parent = nullptr);
    ~mips();

    // 显示显存
    void showVM();
    // 显示寄存器
    void showREG();
    // 显示内存
    void showMEM();

private slots:
    void on_newFile_clicked(); // 按下新建文件

    void on_saveFile_clicked(); // 按下保存文件

    void on_openFile_clicked();

    void textChange();

    void on_compile_clicked();

    void on_run_clicked();

    void on_debug_clicked();

    void on_editReg_clicked();

    void on_runOneLine_clicked();

    void on_memchangeButton_clicked();

    void on_memButton_clicked();

private:
    Ui::mips *ui;
    CodeEditor *editor;

    // 为新建，打开，保存文件使用
    QString nowFileName;
    bool saved;

    // 模拟代码运行使用
    MipsCPU cpu;
    bool debugMode;
    bool isComplied;
    int memPos;
};
#endif // MIPS_H
