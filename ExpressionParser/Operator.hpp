#ifndef OPERATOR_HPP
#define OPERATOR_HPP

enum class Operator : unsigned int {
    ADD
   ,SUB
   ,MUL
   ,DIV
   ,NOP // No Operations
};

#ifdef _DEBUG
const char* ConvertOperatorToString(Operator op);
#endif
Operator ConvertCharToOperator(char op);

#endif