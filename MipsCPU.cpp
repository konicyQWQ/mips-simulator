#include "MipsCPU.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
MipsCPU::MipsCPU()
{
    rgstr[0] = 0; //$zero
    PC = 0;
    for (int i = VMaddr; i < VMaddr + WIDTH * HEIGHT; ++i)
        Memory[VMaddr + i] = '.';
}

void MipsCPU::readMemory()
{
    ifstream in("binary_code.txt");
    string ins;
    int ofs = 0;
    while (in.peek() != EOF)
    {
        in >> ins;
        zjie hi = 0, lo = 0;
        for (int i = 0; i < 16; ++i)
        {
            hi |= ((ins[i] - '0') << (15 - i));
            lo |= ((ins[i + 16] - '0') << (15 - i));
        }
        Memory[ofs] = hi;
        Memory[ofs + 1] = lo;
        ofs += 2;
    }
    //0xFFFFFFFF代表结束
    Memory[ofs] = 0xFFFF;
    Memory[ofs + 1] = 0xFFFF;
}

int MipsCPU::runNext()
{
    word IR;
    int op, rs, rt, rd, dat, sft, fun;
    word adr;
    IR = ((word)Memory[PC] << 16) | (word)Memory[PC + 1];
    if (IR == 0xFFFFFFFF)
        return 1;

    PC += 2;
    op = (IR >> 26) & 63;            //IR{31..26}
    rs = (IR >> 21) & 31;            //IR{25..21}
    rt = (IR >> 16) & 31;            //IR{20..16}
    rd = (IR >> 11) & 31;            //IR{15..11}
    sft = (IR >> 6) & 31;            //IR{11..6}
    fun = IR & 63;                   //IR{5..0}
    dat = (int)(short)(IR & 0xFFFF); //IR{15..0}
    adr = (IR & 0x3FFFFFF) << 1;     //IR{25..0}<<2

    if (op == 0) //R
    {
        if (fun == 32) //add
            rgstr[rd] = rgstr[rs] + rgstr[rt];
        else if (fun == 34) //sub
            rgstr[rd] = rgstr[rs] - rgstr[rt];
        else if (fun == 42) //slt
            rgstr[rd] = (rgstr[rs] < rgstr[rt]) ? 1 : 0;
        else if (fun == 36) //and
            rgstr[rd] = rgstr[rs] & rgstr[rt];
        else if (fun == 37) //or
            rgstr[rd] = rgstr[rs] | rgstr[rt];
        else if (fun == 38) //xor
            rgstr[rd] = rgstr[rs] ^ rgstr[rt];
        else if (fun == 39) //nor
            rgstr[rd] = ~(rgstr[rs] | rgstr[rt]);
        else if (fun == 43) //sltu
            rgstr[rd] = ((word)rgstr[rs] < (word)rgstr[rt]) ? 1 : 0;
        else if (fun == 0) //sll
            rgstr[rd] = rgstr[rs] << sft;
        else if (fun == 2) //srl
            rgstr[rd] = rgstr[rs] >> sft;
        else if (fun == 8) //jr
            PC = rgstr[rs];
        else if (fun == 12)
            ; //TODO: syscall
    }
    else if (op == 8) //addi
        rgstr[rt] = rgstr[rs] + (dword)dat;
    else if (op == 10) //slti
        rgstr[rt] = (rgstr[rs] < dat) ? 1 : 0;
    else if (op == 33) //lh
        rgstr[rt] = (dword)Memory[rgstr[rs] + dat];
    else if (op == 35) //lw
        rgstr[rt] = ((dword)Memory[rgstr[rs] + dat] << 16) | ((dword)Memory[rgstr[rs] + dat] + 1);
    else if (op == 41) //sh
        Memory[rgstr[rs] + dat] = (word)(rgstr[rt] & 0xFFFF);
    else if (op == 43) //sw
    {
        Memory[rgstr[rs] + dat] = (word)(rgstr[rt] >> 16);
        Memory[rgstr[rs] + dat + 1] = (word)(rgstr[rt] & 0xFFFF);
    }
    else if (op == 15) //lui
        rgstr[rt] = dat << 16;
    else if (op == 12) //andi
        rgstr[rt] = rgstr[rs] & dat;
    else if (op == 13) //ori
        rgstr[rt] = rgstr[rs] | dat;
    else if (op == 14) //xori
        rgstr[rt] = rgstr[rs] ^ dat;
    else if (op == 4) //beq
    {
        if (rgstr[rs] == rgstr[rt])
            PC += (dat << 1);
    }
    else if (op == 5) //bne
    {
        if (rgstr[rs] != rgstr[rt])
            PC += (dat << 1);
    }
    else if (op == 2) //j
        PC = (PC & 0xF0000000) | adr;
    else if (op == 3) //jal
    {
        rgstr[31] = PC; //$ra=PC
        PC = (PC & 0xF8000000) + adr;
    }

    //输出显存内容
    cout << endl;
    for (int row = 0; row < HEIGHT; row++)
    {
        for (int col = 0; col < WIDTH; col++)
            printf("%c", Memory[VMaddr + row * WIDTH + col] & 255);

        cout << endl;
    }

    return 0;
}

void MipsCPU::run()
{
    //如果正常结束会返回0
    //遇到全1指令代表结束
    while (!runNext())
        ;
}

void MipsCPU::modifyRegister(int rgstNum, word value)
{
    if (rgstNum != 0)
        rgstr[rgstNum] = value;
}

void MipsCPU::modifyMemory(int ofs, zjie value)
{
    Memory[ofs] = value;
}

zjie MipsCPU::sendMemory(int ofs)
{
    return Memory[ofs];
}

zjie *MipsCPU::sendVM()
{
    return (Memory + VMaddr);
}

word *MipsCPU::sendRegister()
{
    return rgstr;
}