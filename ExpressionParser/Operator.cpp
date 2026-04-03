#include "Operator.hpp"

const char* ConvertOperatorToString(Operator op) {
    switch (op) {
    case Operator::ADD: return "Operator::ADD";
    case Operator::SUB: return "Operator::SUB";
    case Operator::MUL: return "Operator::MUL";
    case Operator::DIV: return "Operator::DIV";
    case Operator::NOP: return "Operator::NOP";
    }
    return "Error";
}

Operator ConvertCharToOperator(char op) {
    switch (op) {
    case '+': return Operator::ADD;
    case '-': return Operator::SUB;
    case '*': return Operator::MUL;
    case '/': return Operator::DIV;
    default : return Operator::NOP;
    }
}
