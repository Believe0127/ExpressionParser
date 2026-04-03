#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "Operator.hpp"

#include <iostream>
#include <concepts>

template <std::integral T>
class Expression {
public:
    Expression(Operator op, Operator nextOp, T left, T right)
        : nextOp(nextOp)
        , op(op)
        , left(left)
        , right(right)
    {}

#ifdef _DEBUG
    void printMembersValue() const noexcept {
        std::cout << "nextOp: " << ConvertOperatorToString(nextOp) << '\n'
                  << "op    : " << ConvertOperatorToString(op)     << '\n'
                  << "left  : " <<  left << '\n'
                  << "right : " << right << "\n\n";
    }
#endif

    inline T Calc() const noexcept {
        switch (op) {
            case Operator::ADD: return left + right;
            case Operator::SUB: return left - right;
            case Operator::MUL: return left * right;
            case Operator::DIV: return left / right;
            case Operator::NOP: return left;
        }
        return 0;
    }

    T Calc(const Expression& expression) {
        switch (nextOp) {
            case Operator::ADD: return Calc() + expression.Calc();
            case Operator::SUB: return Calc() - expression.Calc();
            case Operator::MUL: return Calc() * expression.Calc();
            case Operator::DIV: return Calc() / expression.Calc();
            case Operator::NOP: return 0;
        }
        return 0;
    }

    inline void SetOperator(const Operator op) noexcept {
        this->op = op;
    }

    inline void SetNextOperator(const Operator nextOp) noexcept {
        this->nextOp = nextOp;
    }

    inline Operator GetOperator() const noexcept {
        return op;
    }

    inline Operator GetNextOperator() const noexcept {
        return nextOp;
    }
private:
    Operator  nextOp;
    Operator      op;
    T           left;
    T          right;
};

#endif
