#ifndef STRING_UTIL_HPP
#define STRING_UTIL_HPP

#include <concepts>

namespace StringUtil {

    /*
    * @brief 第一引数に渡された文字列を数値に変換して返す関数
    * @param[in] str 変換元の文字列
    * @return template<std::integral T> 正常に変換できたところまでのポインタを返します。
    */
    template <std::integral T>
    T ToInteger(const char* str) {
        T result = 0;

        const bool isNegativeValue = *str == '-';
        if (isNegativeValue) {
            ++str;
        }

        for (; *str; ++str) {
            if (std::isspace(*str))
                continue;
            else if ('0' <= *str && *str <= '9')
                result = (*str - '0') + (result * 10);
            else
                break;
        }

        return isNegativeValue ? -result : result;
    }

    /*
    * @brief 第一引数に渡された文字列を数値に変換して第二引数の out ポインタに書き込む関数。
    * @param[in] str 変換元の文字列
    * @param[out] out 変換後の数値を書き込むポインタ
    * @return const char* 正常に変換できたところまでのポインタを返します。
    * @details 空白は無視します。文字列に空白または数値以外が文字列に含まれていた場合は変換処理を停止します。
    */
    template <std::integral T>
    const char* ToInteger(const char* str, T* out) {
        T result = 0;

        const bool isNegativeValue = *str == '-';
        if (isNegativeValue) {
            ++str;
        }

        for (; *str; ++str) {
            if (std::isspace(*str))
                continue;
            else if ('0' <= *str && *str <= '9')
                result = (*str - '0') + (result * 10);
            else
                break;
        }

        *out = isNegativeValue ? -result : result;
        return str;
    }

} // namespace StringUtil

#endif 