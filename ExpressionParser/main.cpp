#include <Windows.h>
#include <iostream>
#include <vector>
#include <format>
#include <string>

#include "StringUtil.hpp"
#include "Expression.hpp"
#include "Operator.hpp"

COORD GetConsoleCursorPosition(HANDLE hConsoleOutput) noexcept;

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

template <std::integral T>
T Calculate(const char* expression) {
    std::vector<Expression<T>> expressions;
    expressions.reserve(32);

    Operator op = Operator::NOP;
    T  left     = 0;
    T right     = 0;
    bool hasLeftValue = false;

#ifdef _DEBUG
    std::cout << "Parse Target Expression: " << expression << "\n\n";
#endif

    for (const char* p = expression; *p; ++p) {
        if (std::isspace(static_cast<unsigned char>(*p))) {
            continue;
        }

        p = StringUtil::ToInteger(p, hasLeftValue ? &right : &left);

        if (*p == '\0' || *p == ')') {
            if (hasLeftValue)
                expressions.emplace_back(Expression<T>(op, Operator::NOP, left, right));
            else
                expressions.emplace_back(Expression<T>(Operator::NOP, Operator::NOP, left, 0));
            break;
        }
        else if (*p == '(') { // 括弧内の式の計算     ポインタが先頭の場合は必ず false それ以外は p - 1 が - かどうかを判定する
            const bool hasMinusBeforeParenthesis = p == expression ? false : (*(p - 1) == '-'); 
            std::string inBracketExp;

            p = ExtractInBracketExpression(p, inBracketExp);
            if (!p) {
                throw std::runtime_error("Syntax error.\n");
            }

            T& value = hasLeftValue ? right : left;

            value = Calculate<T>(inBracketExp.c_str());
            if (std::is_signed_v<T> && hasMinusBeforeParenthesis) {
                value *= -1;
            }
            
#ifdef _DEBUG
            std::cout << std::format("括弧内の式の計算結果({0}): {1}\n", hasLeftValue ? "right" : "left", value);
#endif
            if (!hasLeftValue && *p == '\0') {
                return left;
            }
        }

        // 次の演算子 (+ - * /) か文字列終端に当たるまでポインタを進める
        for (;;) {
            if (*p == '\0' || *p == '+' || *p == '-' || *p == '*' || *p == '/') {
                break;
            }
            ++p;
        }

        if (!hasLeftValue) {
            op = ConvertCharToOperator(*p);
            hasLeftValue = true;
        }
        else {
            const Operator nextOp = ConvertCharToOperator(*p);

            if ((op != Operator::MUL && op != Operator::DIV) && (nextOp == Operator::MUL || nextOp == Operator::DIV)) {
                T nextValue = 0;
                p = StringUtil::ToInteger(++p, &nextValue);

#ifdef _DEBUG
                std::cout << "nextnextOp:" << ConvertOperatorToString(ConvertCharToOperator(*p)) << '\n';
#endif

                /***/if (nextOp == Operator::MUL) nextValue *= right;
                else if (nextOp == Operator::DIV) nextValue /= right;
                
                expressions.emplace_back(op, ConvertCharToOperator(*p), left, nextValue);
            } 
            else {
                expressions.emplace_back(op, ConvertCharToOperator(*p), left, right);
            }

            if (*p == '\0') break;

            hasLeftValue  = false;
            op    = Operator::NOP;
            left  = 0;
            right = 0;
        }
    }

#ifdef _DEBUG
    for (const Expression<T>& exp : expressions) {
        exp.printMembersValue();
    }
#endif

    Operator prevOp = Operator::NOP;
    T result        = 0;

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
            ++it;
        }
    }

#ifdef _DEBUG
    std::cout << "計算結果: " << result << "\n\n";
#endif

    return result;
}

int main() {  
    const HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    std::string expression;

    try {
        for (;;) {
            COORD prevCursorPos = GetConsoleCursorPosition(hStdOutput);

            std::getline(std::cin, expression);
            if (expression.empty()) 
                break;

            SetConsoleCursorPosition(hStdOutput, prevCursorPos); // 入力前の位置に戻す

            std::cout << expression << " = " << Calculate<int64_t>(expression.c_str()) << '\n';
        }
    } catch (const std::exception& e) {
        std::cerr << e.what();
    }

    return 0;
}

COORD GetConsoleCursorPosition(HANDLE hConsoleOutput) noexcept {
    CONSOLE_SCREEN_BUFFER_INFO csbi{};

    GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);

    return csbi.dwCursorPosition;
}