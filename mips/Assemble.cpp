//
// Created by sky on 2020/5/30.
//

#include "Assemble.h"
#include "Register.h"
#include "formula.h"
#include <algorithm>
#include <map>
#include <fstream>
#define CALC(s) calcFormula(s+"()")
#define REG(s) "$" + getRegister(s)

string getRegister(string s)
{
    s.erase(0, s.find_first_of('$') + 1);

    if (s[0] == 'z')
        return s.substr(0, 4);
    else
        return s.substr(0, 2);
}

int getRegisterID(const string &r)
{
    auto iter = RegisterID.find(r);
    if (iter != RegisterID.end())
        return iter->second;
    else
        return 0;
}

string toBinary(unsigned x)
{
    string s = "";
    for (int i = 4; i >= 0; --i)
    {
        int b = (x >> i) & 1;
        s.push_back(b + '0');
    }
    return s;
}

string getBinaryRegister(string s)
{

    string id;
    if (s[0] == '$')
    {
        string r = getRegister(s);
        id = toBinary(getRegisterID(r));
    } else
    {
        id = toBinary(stoi(s));
    }
    return id;
}

void Assemble::addInstruction(string ins)
{
    //指令预处理：去掉开头空格和注释，把空格全部换成tab，所有字母全小写
    replace(ins.begin(), ins.end(), '\t', ' ');
    ins.erase(0, ins.find_first_not_of(' '));
    if (ins.find('#') != string::npos)
        ins.erase(ins.find('#'));
    transform(ins.begin(), ins.end(), ins.begin(), ::tolower);
    InstructionSet.push_back(ins);
}

void Assemble::run()
{
    Relocation(InstructionSet);

    for (const auto &ins:InstructionSet)
        process_Instruction(ins);

}

void Assemble::Relocation(vector<string> &ins)
{
    map<string, int> label;

    //将所有label存入map中，并将语句开头的label删除
    for (int i = 0; i < ins.size(); ++i)
    {
        if (ins[i].find(':') != string::npos)
        {
            string lab = ins[i].substr(0, ins[i].find_first_of(':'));
            label.insert(pair<string, int>(lab, i));
            ins[i].erase(0, ins[i].find_first_of(':') + 1);
            ins[i].erase(0, ins[i].find_first_not_of(' '));
        }
    }

    //两层循环
    //对每一条语句，搜索每一个label
    for (int i = 0; i < ins.size(); ++i)
    {
        map<string, int>::iterator iter;
        for (iter = label.begin(); iter != label.end(); ++iter)
        {
            if (ins[i].find(iter->first) != string::npos)
            {
                string op = ins[i].substr(0, ins[i].find_first_of(' '));
                string ofs;
                if (op != "j" && op != "jal")
                {
                    ofs = to_string(iter->second - (i + 1));
                } else
                    ofs = to_string(iter->second);
                int k = ins[i].find(iter->first);
                ins[i].erase(k, k + iter->first.length() - 2);
                ins[i].insert(k, ofs);
            }
        }
    }
}

void Assemble::process_Instruction(string s)
{
    string op = s.substr(0, s.find_first_of(' '));
    s.erase(0, s.find_first_of(' '));
    s.erase(0, s.find_first_not_of(' '));
    if (op == "add" || op == "addu" || op == "sub" || op == "subu" || op == "slt" || op == "sltu"
        || op == "mul"
        || op == "and" || op == "or" || op == "xor" || op == "nor"
        || op == "sll" || op == "srl" || op == "sra" || op == "sllv" || op == "srlv" || op == "srav")
        process_R3(op, s);
    else if (op == "mult" || op == "multu" || op == "div" || op == "divu" || op == "jalr")
        process_R2(op, s);
    else if (op == "jr")
        process_R1(op, s);
    else if (op == "syscall" || op == "eret")
        process_R0(op);
    else if (op == "lui"
             || op == "addi" || op == "addiu"
             || op == "slti" || op == "sltiu"
             || op == "andi" || op == "ori" || op == "xori"
             || op == "beq" || op == "bne")
        process_I3(op, s);
    else if (op == "lw" || op == "lh" || op == "lhu" || op == "sw" || op == "sh")
        process_I2(op, s);
    else if (op == "j" || op == "jal")
        process_J(op, s);
    // 以下皆为伪代码
    else if (op == "move")
    {
        op = "add";
        s.append(", $zero");
        process_R3(op, s);
    }else if (op == "push" || op == "pop")
        process_Inf(op, s);
    else if(op == "inc"){
        string r = REG(s);
        process_I3("addi", r + ", " + r + ", " + "1");
    }else if(op == "dec"){
        string r = REG(s);
        process_I3("addi", r + ", " + r + ", " + "-1");
    }else if(op == "not"){
        string r1 = REG(s), r2;
        s.erase(0, s.find_first_of('$') + 2);
        r2 = REG(s);
        process_R3("nor", r1 + ", " + r2 + ", " + r2);
    }else if(op == "neg"){
        string r1 = REG(s), r2;
        s.erase(0, s.find_first_of('$') + 2);
        r2 = REG(s);
        process_R3("sub", r1 + ", $zero, " + r2);
    }else if(op == "abs"){
        string r1 = REG(s), r2;
        s.erase(0, s.find_first_of('$') + 2);
        r2 = REG(s);
        process_R3("sra", "$at, " + r2 + ", 31");
        process_R3("xor", r1 + ", " + r2 + ", $at");
        process_R3("sub", r1 + ", " + r1 + ", $at");
    }else if(op == "swap"){
        string r1 = REG(s), r2;
        s.erase(0, s.find_first_of('$') + 2);
        r2 = REG(s);
        process_R3("xor", r1 + ", " + r1 + ", " + r2);
        process_R3("xor", r2 + ", " + r1 + ", " + r2);
        process_R3("xor", r1 + ", " + r1 + ", " + r2);
    }else if(op == "b"){
        string num = s.substr(s.find_first_of(' ')+1);
        process_I3("beq", "$zero, $zero, " + num);
    }else if(op == "beqz"){
        string r = REG(s), tag = s.substr(s.find_first_of(',')+1);
        process_I3("beq", r + ", $zero, " + tag);
    }else if(op == "bnez"){
        string r = REG(s), tag = s.substr(s.find_first_of(',')+1);
        process_I3("bne", r + ", $zero, " + tag);
    }else if(op == "beqi" || op == "bnei"){
        string r = REG(s), num;
        s.erase(0, s.find_first_of(',')+1);
        s.erase(0, s.find_first_not_of(' '));
        num = s.substr(0, s.find_first_of(','));
        num.erase(s.find_first_of(','));
        num = to_string(CALC(num));
        s.erase(0, s.find_first_of(',')+1);
        s.erase(0, s.find_first_not_of(' '));
        process_R3("addi", "$at, $zero, " + num);
        process_I3(op == "beqi"? "beq": "bne", "$at," + r + ", " + s);
    }else if(op == "blt"){
        string r1 = REG(s), r2, num;
        s.erase(0, s.find_first_of(','));
        r2 = REG(s);
        s.erase(0, s.find_first_of(','));
        num = to_string(CALC(s));
        process_R3("slt", "$at, " + r1 + ", " + r2);
        process_I3("bne", "$at, $zero, " + num);
    }else if(op == "ble"){
        string r1 = REG(s), r2, num;
        s.erase(0, s.find_first_of(','));
        r2 = REG(s);
        s.erase(0, s.find_first_of(','));
        num = to_string(CALC(s));
        process_R3("slt", "$at, " + r2 + ", " + r1);
        process_I3("beq", "$at, $zero, " + num);
    }else if(op == "bgt"){
        string r1 = REG(s), r2, num;
        s.erase(0, s.find_first_of(','));
        r2 = REG(s);
        s.erase(0, s.find_first_of(','));
        num = to_string(CALC(s));
        process_R3("slt", "$at, " + r2 + ", " + r1);
        process_I3("bne", "$at, $zero, " + num);
    }else if(op == "bge"){
        string r1 = REG(s), r2, num;
        s.erase(0, s.find_first_of(','));
        r2 = REG(s);
        s.erase(0, s.find_first_of(','));
        num = to_string(CALC(s));
        process_R3("slt", "$at, " + r1 + ", " + r2);
        process_I3("beq", "$at, $zero, " + num);
    }else if(op == "seq"){
        string r1 = REG(s), r2, r3;
        s.erase(0, s.find_first_of('$'));
        r2 = REG(s);
        s.erase(0, s.find_first_of('$'));
        r3 = REG(s);
        process_R3("sub", r1 + ", " + r2 + ", " + r3);
        process_I3("sltiu", r1 + ", "+ r2 +", 1");
    }else if(op == "sne"){
        string r1 = REG(s), r2, r3;
        s.erase(0, s.find_first_of('$'));
        r2 = REG(s);
        s.erase(0, s.find_first_of('$'));
        r3 = REG(s);
        process_R3("sub", r1 + ", " + r2 + ", " + r3);
        process_R3("sltu", r1 + ", $zero, " + r1);
    }else if(op == "shi" || op == "shix"){
        string i1, i2, r = REG(s);
        i1 = s.substr(s.find_first_of(' ')+1, s.find_first_of(','));
        s.erase(0, s.find_first_of(',')+1);
        s.erase(0, s.find_first_not_of(' '));
        i2 = s.substr(0, s.find_last_of('('));
        i1 = to_string(CALC(i1));
        i2 = to_string(CALC(i2));
        process_R3("addi", "$at, $zero, " + i1);
        process_I2(op=="shi"?"sh":"shx", "$at," + i2 + "(" + r+")");
    }else if(op == "li" || op == "la"){
        string r = REG(s);
        int num = CALC(s.substr(s.find_first_of(',')+1));
        string numStr = to_string(num);
        if(num < -32768 || num > 32767){
            process_I3("lui", r + ", HIGH " + numStr);
            process_I3("ori", r + ", LOW " + numStr);
        }else
            process_I3("addi", r + ", $zero, " + numStr);
    }
}

void Assemble::process_Inf(const string &op, string s){
    string r, op2 = op == "push"? "sw": "lw";
    vector<string> regs;
    do{
        r = REG(s);
        regs.push_back(r);
        if(s.find_first_of(',') == string::npos)
            s = "";
        else
            s.erase(0, s.find_first_of(',')+1);
    }while(!s.empty());
    if(op == "push")
        process_I3("addi", "$sp, $sp, -" + to_string(regs.size()*2));
    for(int i = 0; i < regs.size(); i++){
        r = regs[i];
        process_I2(op2, r + ", " + to_string(i*2)  + "($sp)");
    }
    if(op == "pop")
        process_I3("addi", "$sp, $sp, " + to_string(regs.size()*2));
}

void Assemble::process_R3(const string &op, string s)
{
    ofstream binary_code("binary_code.txt", ios::app);

    string rd, rs, rt;
    rd = getBinaryRegister(s);
    s.erase(0, s.find_first_of(',') + 1);
    s.erase(0, s.find_first_not_of(' '));

    rs = getBinaryRegister(s);
    s.erase(0, s.find_first_of(',') + 1);
    s.erase(0, s.find_first_not_of(' '));

    rt = getBinaryRegister(s);

    if (op == "add")
    {
        binary_code << "000000" << rs << rt << rd << "00000100000" << endl;
    }
    if (op == "sub")
    {
        binary_code << "000000" << rs << rt << rd << "00000100010" << endl;
    }
    if (op == "slt")
    {
        binary_code << "000000" << rs << rt << rd << "00000101010" << endl;
    }
    if (op == "sltu")
    {
        binary_code << "000000" << rs << rt << rd << "00000101011" << endl;
    }
    if (op == "mul")
    {
        binary_code << "011100" << rs << rt << rd << "00000000010" << endl;
    }
    if (op == "and")
    {
        binary_code << "000000" << rs << rt << rd << "00000100100" << endl;
    }
    if (op == "or")
    {
        binary_code << "000000" << rs << rt << rd << "00000100101" << endl;
    }
    if (op == "xor")
    {
        binary_code << "000000" << rs << rt << rd << "00000100110" << endl;
    }
    if (op == "nor")
    {
        binary_code << "000000" << rs << rt << rd << "00000100111" << endl;
    }
    if (op == "sll")
    {
        binary_code << "000000" << rs << "00000" << rd << rt << "000000" << endl;
    }
    if (op == "sllv")
    {
        binary_code << "000000" << rs << rt << rd << "00000000100" << endl;
    }
    if (op == "srl")
    {
        binary_code << "000000" << rs << "00000" << rd << rt << "00000000010" << endl;
    }
    if (op == "srlv")
    {
        binary_code << "000000" << rs << rt << rd << "00000000110" << endl;
    }
    if (op == "sra")
    {
        binary_code << "000000" << rs << "00000" << rd << rt << "00000000011" << endl;
    }
    if (op == "srav")
    {
        binary_code << "000000" << rs << rt << rd << "00000000111" << endl;
    }

    binary_code.close();
}

void Assemble::process_R2(const string &op, string s)
{
    ofstream binary_code("binary_code.txt", ios::app);

    string rs, rt;
    rs = getBinaryRegister(s);
    s.erase(0, s.find_first_of(',') + 1);
    s.erase(0, s.find_first_not_of(' '));

    rt = getBinaryRegister(s);

    if (op == "mult")
    {
        binary_code << "000000" << rs << rt << "00000" << "00000011000";
    }
    if (op == "multu")
    {
        binary_code << "000000" << rs << rt << "00000" << "00000011001";
    }
    if (op == "div")
    {
        binary_code << "000000" << rs << rt << "00000" << "00000011010";
    }
    if (op == "divu")
    {
        binary_code << "000000" << rs << rt << "00000" << "00000011011";
    }
    if (op == "jalr")
    {
        binary_code << "00000" << rs << "00000" << rt << "00000001001";
    }
    binary_code << endl;

    binary_code.close();
}

void Assemble::process_R1(const string &op, string s)
{
    ofstream binary_code("binary_code.txt", ios::app);

    string rs;
    rs = getBinaryRegister(std::move(s));
    if (op == "jr")
    {
        binary_code << "000000" << rs << "00000" << "00000" << "00000" << "001000" << endl;
    }

    binary_code.close();
}

void Assemble::process_R0(const string &op)
{
    ofstream binary_code("binary_code.txt", ios::app);

    if (op == "syscall")
        binary_code << "00000000000000000000000000001100" << endl;
    if (op == "eret")
        binary_code << "01000010000000000000000000011000" << endl;

    binary_code.close();
}
#include <iostream>
void Assemble::process_I2(const string &op, string s)
{
    ofstream binary_code("binary_code.txt", ios::app);

    string rt = getBinaryRegister(s);
    s.erase(0, s.find_first_of(',') + 1);
    s.erase(0, s.find_first_not_of(' '));

    string rs = getBinaryRegister(s.substr(s.find_first_of('(') + 1));

    int imm = calcFormula(s);
    string b_imm;
    for (int i = 15; i >= 0; --i)
        b_imm.push_back(((imm >> i) & 1) + '0');

    if (op == "lw")
    {
        binary_code << "100011" << rs << rt << b_imm;
    }
    if (op == "lh")
    {
        binary_code << "100001" << rs << rt << b_imm;
    }
    if (op == "lhu")
    {
        binary_code << "100101" << rs << rt << b_imm;
    }
    if (op == "sw")
    {
        binary_code << "101011" << rs << rt << b_imm;
    }
    if (op == "sh")
    {
        binary_code << "101001" << rs << rt << b_imm;
    }
    binary_code << endl;

    binary_code.close();
}

void Assemble::process_I3(const string &op, string s)
{
    ofstream binary_code("binary_code.txt", ios::app);

    if (op == "lui")
    {
        string rt = getBinaryRegister(s);
        s.erase(0, s.find_first_of(',') + 1);
        s.erase(0, s.find_first_not_of(' '));
        string b_imm;
        int imm = stoi(s);
        for (int i = 15; i >= 0; --i)
        {
            int b = (imm >> i) & 1;
            b_imm.push_back(b + '0');
        }
        binary_code << "001111" << "00000" << rt << b_imm << endl;
        return;
    }
    string rs, rt;
    rt = getBinaryRegister(s);
    s.erase(0, s.find_first_of(',') + 1);
    s.erase(0, s.find_first_not_of(' '));
    rs = getBinaryRegister(s);
    s.erase(0, s.find_first_of(',') + 1);
    s.erase(0, s.find_first_not_of(' '));
    string b_imm;
    int imm = stoi(s);
    for (int i = 15; i >= 0; --i)
    {
        int b = (imm >> i) & 1;
        b_imm.push_back(b + '0');
    }

    if (op == "addi")
    {
        binary_code << "001000" << rs << rt << b_imm;
    }
    if (op == "addiu")
    {
        binary_code << "001001" << rs << rt << b_imm;
    }
    if (op == "slti")
    {
        binary_code << "001010" << rs << rt << b_imm;
    }
    if (op == "sltiu")
    {
        binary_code << "001011" << rs << rt << b_imm;
    }
    if (op == "andi")
    {
        binary_code << "001100" << rs << rt << b_imm;
    }
    if (op == "ori")
    {
        binary_code << "001101" << rs << rt << b_imm;
    }
    if (op == "xori")
    {
        binary_code << "001110" << rs << rt << b_imm;
    }
    if (op == "bne")
    {
        binary_code << "000101" << rs << rt << b_imm;
    }
    if (op == "beq")
    {
        binary_code << "000100" << rs << rt << b_imm;
    }
    binary_code << endl;

    binary_code.close();
}

void Assemble::process_J(const string &op, const string &s)
{
    ofstream binary_code("binary_code.txt", ios::app);

    binary_code << "000010";
    if (op == "J")
    {
        binary_code << "000010";
    } else if (op == "jal")
    {
        binary_code << "000011";
    }
    string addr;
    int ofs = stoi(s);
    for (int i = 25; i >= 0; --i)
    {
        int b = (ofs >> i) & 1;
        addr.push_back(b + '0');
    }
    binary_code << addr << endl;

    binary_code.close();
}