#ifndef MIPSSIMULATOR_MIPSCPU_H
#define MIPSSIMULATOR_MIPSCPU_H

#include "typedef.h"
#define MAXMEM 4096 //内存大小，4K个zjie相当于8KB
#define VMaddr 1024 //显存开始的位置
#define WIDTH 40
#define HEIGHT 20

class MipsCPU
{
private:
    word PC;
    word rgstr[32]; //不知道为啥不让用register做变量名
    zjie Memory[MAXMEM];

public:
    MipsCPU();

    int readMemory(); //会从binary_code.txt里面读指令,会把文本转成二进制
    int runNext();     //单步运行
    void run();        //直接运行

    zjie *sendVM();                               //给界面传显存
    void modifyMemory(int ofs, zjie value);       //修改某处内存（按zjie）
    void modifyRegister(int rgstNum, word value); //修改某个寄存器的值
    zjie sendMemory(int ofs);                     //给界面传某处内存的值（按zjie）
    word *sendRegister();                         //给界面传全部寄存器的值
};

#endif
