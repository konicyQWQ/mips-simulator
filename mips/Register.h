//
// Created by sky on 2020/3/6.
//

#ifndef ASSEMBLE_INTERPRETATION_REGISTER_H
#define ASSEMBLE_INTERPRETATION_REGISTER_H

#include "iostream"
#include <map>
#include <string>

extern map<std::string, int> RegisterID;
extern std::string RegisterName[32];
/*
    const int zero = 0;
    int at; 1
    int v0, v1; 2 3
    int a0, a1, a2, a3; 4 5 6 7
    int t0, t1, t2, t3, t4, t5, t6, t7; 8 9 10 11 12 13 14 15
    int s0, s1, s2, s3, s4, s5, s6, s7; 16 17 18 19 20 21 22 23
    int t8, t9; 24 25
    int k0, k1; 26 27
    int gp; 28
    int sp; 29
    int fp; 30
    int ra; 31
*/

#endif //ASSEMBLE_INTERPRETATION_REGISTER_H
