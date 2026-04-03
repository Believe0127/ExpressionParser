#ifndef EXPRESSION_PARSER
#define EXPRESSION_PARSER

#include <string>
#include <vector>

#include "StringUtil.hpp"
#include "Expression.hpp"
#include "Operator.hpp"

template <class T>
class ExpressionParser {
    // 渡された括弧 ('(') から始まる文字列から括弧内の数式を抜き出す関数
    const char* ExtractInBracketExpression(const char* p, std::string& out) {
        if (*p != '(') return p;

        const char* const start = p + 1;
        int32_t depth = 0;

        for (const char* ptr = p; *ptr; ++ptr) {
            /***/if (*ptr == '(') ++depth;
            else if (*ptr == ')') --depth;

            if (depth == 0) {
                out.assign(start, ptr);
                return ptr + 1; // 括弧の一つ右
            }
        }
        return nullptr;
    }
public:
    ExpressionParser(const std::string& expression) 
        : expression(expression)
        , op(Operator::NOP)
        , left(0)
        , right(0)
        , hasLeftValue(false)
    {}

    T Calculate() {
#ifdef _DEBUG
        for (const Expression<T>& exp : expressions) {
            exp.printMembersValue();
        }
#endif

        Parse();

        Operator prevOp = Operator::NOP;
        T result = 0;

        for (auto it = expressions.begin(), end = expressions.end(); it != end; ++it) {
            const auto nextIt           = std::next(it);
            const bool isLastExpression = nextIt == end;

            const T calcResult = isLastExpression ? it->Calc() : it->Calc(*nextIt);

            if (prevOp == Operator::NOP) // 初回の計算
                result = calcResult;
            else {
                switch (prevOp) {
                    case Operator::ADD: result += calcResult; break;
                    case Operator::SUB: result -= calcResult; break;
                    case Operator::MUL: result *= calcResult; break;
                    case Operator::DIV: result /= calcResult; break;
                    case Operator::NOP: break;
                }
            }

            if (!isLastExpression) {
                prevOp = nextIt->GetNextOperator();
            }
        }

#ifdef _DEBUG
        std::cout << "計算結果: " << result << '\n';
#endif

        return result;
    }

    void Parse() {      
        expressions.reserve(32);

        std::cout << "Parse Target Expression: " << expression << "\n\n";

        for (const char* p = expression.c_str(); *p; ++p) {
            if (std::isspace(*p)) {
                continue;
            }

            if (!hasLeftValue) {
                p = StringUtil::ToInteger(p, &left);

                if (*p == '\0') {
                    expressions.emplace_back(Expression<T>(Operator::NOP, Operator::NOP, left, 0));
                    break;
                } else if (*p == '(') {
                    std::string inBracketExp;
                    p = ExtractInBracketExpression(p, inBracketExp);

                    left = ExpressionParser<T>(inBracketExp).Calculate();
                    std::cout << "括弧内の式の計算結果 (left): " << left << '\n';

                    if (*p == '\0') return left;
                }

                // 次の演算子 (+ - * /) か文字列終端に当たるまでポインタを進める
                for (;;) {
                    if (*p == '\0' || *p == '+' || *p == '-' || *p == '*' || *p == '/') {
                        break;
                    }
                    ++p;
                }

                op = ConvertCharToOperator(*p);
                hasLeftValue = true;
            } 
            else {
                p = StringUtil::ToInteger(p, &right);

                if (*p == '\0' || *p == ')') { // 終端文字または括弧閉じの場合は emplace_back して終了
                    expressions.emplace_back(Expression<T>(op, Operator::NOP, left, right));
                    break;
                }
                else if (*p == '(') {
                    std::string inBracketExp;
                    p = ExtractInBracketExpression(p, inBracketExp);

                    right = ExpressionParser<T>(inBracketExp).Calculate();

                    #ifdef _DEBUG
                        std::cout << "括弧内の式の計算結果 (right): " << right << '\n';
                    #endif
                }

                // 次の演算子 (+ - * /) か文字列終端に当たるまでポインタを進める
                for (;;) {
                    if (*p == '\0' || *p == '+' || *p == '-' || *p == '*' || *p == '/') {
                        break;
                    }
                    ++p;
                }

                const Operator nextOp = ConvertCharToOperator(*p);

                if ((op != Operator::MUL && op != Operator::DIV) && (nextOp == Operator::MUL || nextOp == Operator::DIV)) {
                    T nextValue = 0;
                    p = StringUtil::ToInteger(++p, &nextValue);
#ifdef _DEBUG
                    std::cout << "nextnextOp:" << ConvertOperatorToString(ConvertCharToOperator(*p)) << '\n';
#endif
                    if (nextOp == Operator::MUL)
                        nextValue = right * nextValue;
                    else if (nextOp == Operator::DIV)
                        nextValue = right / nextValue;

                    expressions.emplace_back(op, ConvertCharToOperator(*p), left, nextValue);

                    if (*p == '\0') break;
                }
                else {
                    expressions.emplace_back(op, ConvertCharToOperator(*p), left, right);
                    if (*p == '\0') break;
                }

                hasLeftValue  = false;
                op    = Operator::NOP;
                left  = 0;
                right = 0;
            }
        }
    }
private:
    std::string expression;

    std::vector<Expression<T>> expressions;
    Operator op;
    T  left;
    T right;
    bool hasLeftValue;
};

#endif