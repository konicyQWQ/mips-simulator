//
// Created by sky on 2020/5/30.
//

#ifndef ASSEMBLE_INTERPRETATION_ASSEMBLE_H
#define ASSEMBLE_INTERPRETATION_ASSEMBLE_H

#include <vector>
#include <string>

#ifndef DEBUG
#define DEBUG
#include <iostream>
#endif

using namespace std;

class Assemble
{
private:
    vector<string> InstructionSet;

    void Relocation(vector<string> &ins);

    void process_Instruction(string s);

    void process_R3(const string &op, string s);

    void process_R2(const string &op, string s);

    void process_R1(const string &op, string s);

    void process_R0(const string &op);

    void process_I2(const string &op, string s);

    void process_I3(const string &op, string s);

    void process_J(const string &op, const string &s);

    void process_Inf(const string &op, string s);

public:
    void addInstruction(string ins);

    void run();
};


#endif //ASSEMBLE_INTERPRETATION_ASSEMBLE_H
