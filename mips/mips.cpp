#include "mips.h"
#include "ui_mips.h"

#include <QHBoxLayout>
#include <QFileDialog>
#include <QPalette>
#include <QTextStream>
#include "MipsCPU.h"
#include "typedef.h"
#include "Assemble.h"
#include "Disassemble.h"
#include <fstream>
#include <QDebug>
#include <QLineEdit>
#include "MipsCPU.h"
#include <QInputDialog>
using namespace std;

map<string, int> RegisterID = {
        {"zero", 0},
        {"at",   1},
        {"v0",   2},
        {"v1",   3},
        {"a0",   4},
        {"a1",   5},
        {"a2",   6},
        {"a3",   7},
        {"t0",   8},
        {"t1",   9},
        {"t2",   10},
        {"t3",   11},
        {"t4",   12},
        {"t5",   13},
        {"t6",   14},
        {"t7",   15},
        {"s0",   16},
        {"s1",   17},
        {"s2",   18},
        {"s3",   19},
        {"s4",   20},
        {"s5",   21},
        {"s6",   22},
        {"s7",   23},
        {"t8",   24},
        {"t9",   25},
        {"k0",   26},
        {"k1",   27},
        {"gp",   28},
        {"sp",   29},
        {"fp",   30},
        {"ra",   31}
};

string RegisterName[32] = {     /* NOLINT */
        "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
        "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
        "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
        "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra",
};

mips::mips(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mips)
    , editor(new CodeEditor)
{
    ui->setupUi(this);

    // 把mips代码编辑器放上去
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(editor);
    layout->setMargin(0);
    ui->codeContainer->setLayout(layout);

    // 初始化完成
    ui->consoleOutput->appendPlainText("初始化完成...");
    ui->tabArea->setCurrentIndex(0);
    // 一输入就改变已保存状态为false
    connect(editor, CodeEditor::textChanged, this, mips::textChange);

    // 代码在 editor 里面 editor->toPlainText()
    // 往控制台显示命令 ui->consoleOutput->appendPlainText("xxxxx");

}

mips::~mips()
{
    delete editor;
    delete ui;
}

// ===========================新建文件，打开文件，保存文件部分================

void mips::textChange() {
    if(editor->toPlainText() == "")
        this->saved = true;
    else
        this->saved = false;
}

void mips::on_newFile_clicked()
{
    if(!this->saved) {
        ui->consoleOutput->appendPlainText("请先保存!");
    } else {
        this->nowFileName = "";
        this->saved = true;
        editor->clear();
        ui->consoleOutput->appendPlainText("新建文件完成!");
    }
    ui->tabArea->setCurrentIndex(0);
}

void mips::on_saveFile_clicked()
{
    if(this->nowFileName == "") {
        QFileDialog fileDialog;
        this->nowFileName = fileDialog.getSaveFileName(this, "Open File", "", "Asm File(*.asm)");
        if(this->nowFileName == "") {
            ui->consoleOutput->appendPlainText("保存文件失败!");
            return;
        }
    }
    QFile file(this->nowFileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream textStream(&file);
    QString str = editor->toPlainText();
    textStream << str;
    file.close();
    this->saved = true;

    ui->consoleOutput->appendPlainText("保存文件成功!");
    ui->tabArea->setCurrentIndex(0);
}

void mips::on_openFile_clicked()
{
    QFileDialog fileDialog;
    this->nowFileName = fileDialog.getOpenFileName(this, "Open File", "", "Asm File(*.asm)");
    if(this->nowFileName == "") {
        ui->consoleOutput->appendPlainText("打开文件失败!");
        return;
    }
    QFile file(this->nowFileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream textStream(&file);
    while(!textStream.atEnd()) {
        editor->setPlainText(textStream.readAll());
    }
    this->saved = true;
    file.close();
    ui->consoleOutput->appendPlainText("打开文件成功!");
    ui->tabArea->setCurrentIndex(0);
}

// ==============================编译代码==============================

void mips::on_compile_clicked()
{
    ofstream binary_code("binary_code.txt", ios::trunc);
    binary_code.close();
    Assemble a;
    string str = editor->toPlainText().toStdString() + '\n';

    int pos1 = str.find(".data");
    int pos2 = str.find(".text");
    string variable = str.substr(pos1 + 5, pos2 - pos1 - 5);
    string ins = str.substr(pos2 + 5);

    qDebug() << QString::fromStdString(variable);
    qDebug() << QString::fromStdString(ins);

    while (true)
    {
        int pos = ins.find('\n');
        if(pos == string::npos) {
            break;
        }
        string s = ins.substr(0, pos);
        ins = ins.substr(pos+1);
        a.addInstruction(s);
    }
    a.run();
    ui->consoleOutput->appendPlainText("编译成功!");
    ui->tabArea->setCurrentIndex(0);
}

// ==============================运行代码==============================

void mips::on_run_clicked()
{
    if(editor->toPlainText() == "") {
        this->ui->consoleOutput->appendPlainText("没有代码?");
        ui->tabArea->setCurrentIndex(0);
        return;
    }
    if(this->debugMode == true) {
        this->debugMode = false;
    }
    if(cpu.readMemory() != 0) {
        this->ui->consoleOutput->appendPlainText("请先点击编译!");
        ui->tabArea->setCurrentIndex(0);
    } else {
        this->ui->consoleOutput->appendPlainText("运行开始!");
        cpu.run();
        this->showVM();
        this->showREG();
        this->showMEM();
        this->ui->consoleOutput->appendPlainText("运行结束!");
        ui->tabArea->setCurrentIndex(2);
    }
}

// ==============================调试代码==============================

void mips::on_debug_clicked()
{
    if(editor->toPlainText() == "") {
        this->ui->consoleOutput->appendPlainText("没有代码?");
        ui->tabArea->setCurrentIndex(0);
        return;
    }
    if(cpu.readMemory() != 0) {
        this->ui->consoleOutput->appendPlainText("请先点击编译!");
    } else {
        this->ui->consoleOutput->appendPlainText("调试开始!");
        this->showREG();
        this->showVM();
        this->showMEM();
        this->debugMode = true; // 调试模式开始中
        ui->tabArea->setCurrentIndex(1);
    }
}

// ============================== 设置寄存器 ===========================

void mips::on_editReg_clicked()
{
    if(this->debugMode == false) {
        return;
    }
    int idx = ui->regNumber->text().toInt();
    word value = (word)ui->number->text().toUInt();
    if(idx < 0 || idx >= 32)
        return;
    cpu.modifyRegister(idx, value);

    this->showREG();
}

// ============================= 单步运行 =============================
void mips::on_runOneLine_clicked()
{
    if(this->debugMode == false)
        return ;
    int errCode = cpu.runNext();
    if(errCode == 1) {
        // 运行结束
        this->debugMode = false;
    }
    this->showREG();
    this->showMEM();
    this->showVM();
    ui->tabArea->setCurrentIndex(1);
}

// ============================= 内部使用的函数 =========================

void mips::showVM() {
    zjie *vm = cpu.sendVM();
    string str;
    for(int i=0; i<HEIGHT; i++) {
        for(int j=0; j<WIDTH; j++)
            str += (char)vm[i * WIDTH + j];
        str += '\n';
    }
    ui->vmOutput->setPlainText(QString::fromStdString(str));
}

void mips::showREG() {
    word *reg = cpu.sendRegister();
    QString str;
    str += QString("$zero($0):\t") + QString::number(reg[0], 10) + "\n";
    str += QString("$at($1):\t") + QString::number(reg[1], 10) + "\n";
    str += QString("$v0($2):\t") + QString::number(reg[2], 10) + "\n";
    str += QString("$v1($3):\t") + QString::number(reg[3], 10) + "\n";
    str += QString("$a0($4):\t") + QString::number(reg[4], 10) + "\n";
    str += QString("$a1($5):\t") + QString::number(reg[5], 10) + "\n";
    str += QString("$a2($6):\t") + QString::number(reg[6], 10) + "\n";
    str += QString("$a3($7):\t") + QString::number(reg[7], 10) + "\n";
    for(int i=8; i<=15; i++)
        str += QString("$t") + QString::number(i-8, 10) + "($" + QString::number(i, 10) + "):\t" + QString::number(reg[i], 10) + "\n";
    for(int i=16; i<=23; i++)
        str += QString("$s") + QString::number(i-16, 10) + "($" + QString::number(i, 10) + "):\t" + QString::number(reg[i], 10) + "\n";
    str += QString("$t8($24):\t") + QString::number(reg[24], 10) + "\n";
    str += QString("$t9($25):\t") + QString::number(reg[25], 10) + "\n";
    str += QString("$k0($26):\t") + QString::number(reg[26], 10) + "\n";
    str += QString("$k1($27):\t") + QString::number(reg[27], 10) + "\n";
    str += QString("$gp($28):\t") + QString::number(reg[28], 10) + "\n";
    str += QString("$sp($29):\t") + QString::number(reg[29], 10) + "\n";
    str += QString("$fp($30):\t") + QString::number(reg[30], 10) + "\n";
    str += QString("$ra($31):\t") + QString::number(reg[31], 10) + "\n";
    ui->regCode->setPlainText(str);
}

void mips::showMEM() {
    QString str;
    for(int i=0; i<=7; i++)
        str += "\t+" + QString::number(i, 16);
    str += '\n';
    for(int i=0; i<=6; i++) {
        str += QString::number(this->memPos + i * 8, 10);
        for(int j=0; j<=7; j++) {
            QString val = QString::number(cpu.sendMemory(this->memPos + i * 8 + j), 16);

            str += "\t" + QString(4 - val.length(), '0') + val;
        }
        str += '\n';
    }
    ui->memOutput->setPlainText(str);

    Disassemble d;
    zjie a = cpu.sendMemory(cpu.PC);
    zjie b = cpu.sendMemory(cpu.PC+1);

    QString first = QString::number(a, 2); first = QString(16 - first.length(), '0') + first;
    QString second = QString::number(b, 2); second = QString(16 - second.length(), '0') + second;
    QString qs = first + second;
    d.addInstruction(qs.toStdString());

    std::string pcstr = std::string("PC: ") + QString::number(cpu.PC).toStdString() + "\n" + std::string("now instruction: ") + d.run();
    ui->PC->setPlainText(QString::fromStdString(pcstr));
}

// ================== 内存修改 ==================

void mips::on_memchangeButton_clicked()
{
    int id = ui->memAddress->text().toInt();
    if(id < 0 || id > 4096)
        return;
    zjie val = ui->memValue->text().toUInt(nullptr, 16);
    cpu.modifyMemory(id, val);
    this->showMEM();
    this->showVM();
}

// =================== 内存跳转 ===================

void mips::on_memButton_clicked()
{
    int pos = ui->memPosition->text().toInt();
    if(pos < 0 || pos > 4096)
        return;
    this->memPos = pos;
    this->showMEM();
}

// ==================== 打开一个输入字符串的对话框 ===============
QString mips::inputStringDialog(QString title, QString label) {
    QLineEdit::EchoMode echomode = QLineEdit::Normal;
    bool ok = false;
    QString text = QInputDialog::getText(nullptr, title, label, echomode, "", &ok);
    if(ok && !text.isEmpty())
        return text;
    else
        return "0";
}
