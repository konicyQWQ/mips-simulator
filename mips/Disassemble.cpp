//
// Created by sky on 2020/5/30.
//

#include "Disassemble.h"
#include "Register.h"
#include <fstream>
#include <iostream>

int Bin2Int(string s, int b)
{
    int wei = 1;
    int ret = 0;
    for (int i = b - 1; i >= 0; --i)
    {
        ret += wei * (s[i] - '0');
        wei *= 2;
    }
    return ret;
}

string Disassemble::interprete(const string &s)
{
    if (s == "00000000000000000000000000001100")
        return "syscall";
    if (s == "01000000000000000000000000011000")
        return "erec";
    if (s.substr(0, 6) == "000000")
    {
        string fun = s.substr(26, 6);
        int rs, rt, rd;
        rs = Bin2Int(s.substr(6, 5), 5);
        rt = Bin2Int(s.substr(11, 5), 5);
        rd = Bin2Int(s.substr(16, 5), 5);
        if (fun == "100000")
        {
            return "add\t$" + RegisterName[rd] + ", $" + RegisterName[rs] + ", $" + RegisterName[rt];
        }
        if (fun == "100010")
        {
            return "sub\t$" + RegisterName[rd] + ", $" + RegisterName[rs] + ", $" + RegisterName[rt];
        }
        if (fun == "101010")
        {
            return "slt\t$" + RegisterName[rd] + ", $" + RegisterName[rs] + ", $" + RegisterName[rt];
        }
        if (fun == "101011")
        {
            return "sltu\t$" + RegisterName[rd] + ", $" + RegisterName[rs] + ", $" + RegisterName[rt];
        }
        if (fun == "100000")
        {
            return "add\t$" + RegisterName[rd] + ", $" + RegisterName[rs] + ", $" + RegisterName[rt];
        }
        if (fun == "100100")
        {
            return "and\t$" + RegisterName[rd] + ", $" + RegisterName[rs] + ", $" + RegisterName[rt];
        }
        if (fun == "100101")
        {
            return "or\t$" + RegisterName[rd] + ", $" + RegisterName[rs] + ", $" + RegisterName[rt];
        }
        if (fun == "100110")
        {
            return "xor\t$" + RegisterName[rd] + ", $" + RegisterName[rs] + ", $" + RegisterName[rt];
        }
        if (fun == "100111")
        {
            return "nor\t$" + RegisterName[rd] + ", $" + RegisterName[rs] + ", $" + RegisterName[rt];
        }
        if (fun == "001000")
        {
            return "jr\t$" + RegisterName[rs];
        }
    } else if (s.substr(0, 6) == "000010" || s.substr(0, 6) == "000011")
    {
        int ofs = Bin2Int(s.substr(6, 26), 26);
        if (s.substr(0, 6) == "000010")
            return "J\t" + to_string(ofs);
        else
            return "Jal\t" + to_string(ofs);
    } else
    {
        int imm = 0;
        imm = Bin2Int(s.substr(16, 16), 16);
        int rs = Bin2Int(s.substr(6, 5), 5);
        int rt = Bin2Int(s.substr(11, 5), 5);
        string op = s.substr(0, 6);
        string s_imm = to_string(imm);
        if (op == "001111")
        {
            return "lui\t$" + RegisterName[rt] + ", " + s_imm;
        }
        if (op == "100011")
        {
            if (imm != 0)
                return "lw\t$" + RegisterName[rt] + ", " + s_imm + "($" + RegisterName[rs] + ")";
            else
                return "lw\t$" + RegisterName[rt] + ", $" + RegisterName[rs];
        }
        if (op == "100001")
        {
            if (imm != 0)
                return "lh\t$" + RegisterName[rt] + ", " + s_imm + "($" + RegisterName[rs] + ")";
            else
                return "lh\t$" + RegisterName[rt] + ", $" + RegisterName[rs];
        }
        if (op == "100101")
        {
            if (imm != 0)
                return "lhu\t$" + RegisterName[rt] + ", " + s_imm + "($" + RegisterName[rs] + ")";
            else
                return "lhu\t$" + RegisterName[rt] + ", $" + RegisterName[rs];
        }
        if (op == "101011")
        {
            if (imm != 0)
                return "sw\t$" + RegisterName[rt] + ", " + s_imm + "($" + RegisterName[rs] + ")";
            else
                return "sw\t$" + RegisterName[rt] + ", $" + RegisterName[rs];
        }
        if (op == "101001")
        {
            if (imm != 0)
                return "sh\t$" + RegisterName[rt] + ", " + s_imm + "($" + RegisterName[rs] + ")";
            else
                return "sh\t$" + RegisterName[rt] + ", $" + RegisterName[rs];
        }
        if (op == "001000")
        {
            return "addi\t$" + RegisterName[rt] + ", $" + RegisterName[rs] + ", " + s_imm;
        }
        if (op == "001010")
        {
            return "slti\t$" + RegisterName[rt] + ", $" + RegisterName[rs] + ", " + s_imm;
        }
        if (op == "001011")
        {
            return "sltiu\t$" + RegisterName[rt] + ", $" + RegisterName[rs] + ", " + s_imm;
        }
        if (op == "001100")
        {
            return "andi\t$" + RegisterName[rt] + ", $" + RegisterName[rs] + ", " + s_imm;
        }
        if (op == "001101")
        {
            return "ori\t$" + RegisterName[rt] + ", $" + RegisterName[rs] + ", " + s_imm;
        }
        if (op == "001110")
        {
            return "xori\t$" + RegisterName[rt] + ", $" + RegisterName[rs] + ", " + s_imm;
        }
        if (op == "000100")
        {
            return "beq\t$" + RegisterName[rt] + ", $" + RegisterName[rs] + ", " + s_imm;
        }
        if (op == "000101")
        {
            return "bne\t$" + RegisterName[rt] + ", $" + RegisterName[rs] + ", " + s_imm;
        }
    }
    return "unknown";
}

void Disassemble::addInstruction(string s)
{
    BinaryCodeSet.push_back(s);
}

void Disassemble::clear() {
    BinaryCodeSet.clear();
}

string Disassemble::run()
{
    //BinaryCodeSet.erase(BinaryCodeSet.begin());
    for (const auto &i : BinaryCodeSet) {

        InstructionSet.push_back(interprete(i));
    }

    //ofstream assemble_code("assemble_code.txt", ios::app);
    string a;
    for (const auto &i:InstructionSet)
        a += i + '\n';
    return a;
    //assemble_code.close();
}
