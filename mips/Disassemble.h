//
// Created by sky on 2020/5/30.
//

#ifndef ASSEMBLE_INTERPRETATION_DISASSEMBLE_H
#define ASSEMBLE_INTERPRETATION_DISASSEMBLE_H

#include <string>
#include <vector>

using namespace std;

class Disassemble
{
private:
    vector<string> BinaryCodeSet, InstructionSet;

    string interprete(const string &s);

public:
    void addInstruction(string s);
    void clear();

    string run();
};


#endif //ASSEMBLE_INTERPRETATION_DISASSEMBLE_H
