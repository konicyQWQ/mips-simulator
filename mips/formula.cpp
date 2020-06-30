#include "formula.h"
#include <string>
#include <iostream>
using namespace std;

static int opStack[110], numberStack[110];
static int opTop, numberTop;
static int opValue[256];

// 用数字栈顶的两个数字和符号栈顶的符号运算
void xopy() {
    int x = numberStack[numberTop - 1];
    int y = numberStack[numberTop];
    char op = opStack[opTop];
    opTop--;
    numberTop -= 2;
    if(op == '+') numberStack[++numberTop] = x + y;
    if(op == '-') numberStack[++numberTop] = x - y;
    if(op == '*') numberStack[++numberTop] = x * y;
    if(op == '/') numberStack[++numberTop] = x / y;
}

int calcFormula(string formula) {
    // 第一步：删除(xxx)
    formula.erase(formula.find_last_of('('), formula.find_last_of(')'));
    // 第二步：删除无关字符
    int pos = 0;
    while((pos = formula.find(' ')) != string::npos) formula.erase(pos, 1);
    while((pos = formula.find('\t')) != string::npos) formula.erase(pos, 1);
    // 第三步：初始值设置
    opTop = 0; numberTop = 0;
    opValue['+'] = 1; opValue['-'] = 1;
    opValue['*'] = 2; opValue['/'] = 2;
    // 第四步：转换表达式
    for(int i = 0; i < formula.length(); i++) {
        if(formula[i] >= '0' && formula[i] <= '9') {  // 数字
            int x = 0;
            while(formula[i] >= '0' && formula[i] <= '9') {
                x = x * 10 + formula[i] - '0';
                i++;
            }
            numberStack[++numberTop] = x;
            i--;
        } else if(formula[i] == '+' || formula[i] == '-' || formula[i] == '*' || formula[i] == '/' 
                    || formula[i] == '(' || formula[i] == ')') {     // 符号
            if(formula[i] == '(' || opTop == 0)
                opStack[++opTop] = formula[i];
            else if(formula[i] == ')') {
                while(opTop && opStack[opTop] != '(')
                    xopy();
                opTop--;
            } else if(opValue[formula[i]] < opValue[opStack[opTop]]) {
                while(opTop && opValue[formula[i]] < opValue[opStack[opTop]])
                    xopy();
                opStack[++opTop] = formula[i];
            } else {
                opStack[++opTop] = formula[i];
            }
        }
    }
    // 第5步：运算
    while(opTop)
        xopy();
    return numberStack[1];
}