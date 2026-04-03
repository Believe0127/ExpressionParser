#ifndef OPERATOR_HPP
#define OPERATOR_HPP

enum class Operator : unsigned int {
    ADD
   ,SUB
   ,MUL
   ,DIV
   ,NOP // No Operations
};

const char* ConvertOperatorToString(Operator op);
Operator ConvertCharToOperator(char op);

#endif