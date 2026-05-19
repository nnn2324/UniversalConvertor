#pragma once
#include "BigFraction.h"
#include <string>
#include <map>
#include <stdexcept>

class BaseFormatter {
private:
    // перевод отдельного разряда в строковое представление
    static std::string digitToString(int digit) {
        if (digit >= 0 && digit <= 9) return std::to_string(digit);
        if (digit >= 10 && digit <= 35) return std::string(1, 'A' + (digit - 10));
        return "[" + std::to_string(digit) + "]";
    }

public:
    static std::string format(BigFraction fraction, int q, bool& is_trimmed) {
        const size_t MAX_OUTPUT_LENGTH = 100000; // ограничение
        is_trimmed = false;

        BigInteger integerPart = fraction.getIntegerPart();
        BigInteger remainder = fraction.getRemainder();

        // формируем целую часть числа
        std::string intStr = "";
        if (integerPart.isZero()) {
            intStr = "0";
        } else {
            BigInteger temp = integerPart;
            while (!temp.isZero()) {
                BigInteger rem = temp % q;
                int digit = std::stoi(rem.toString());
                intStr = digitToString(digit) + intStr;
                temp /= q;
            }
        }

        if (remainder.isZero()) return intStr;

        // алгоритм длинного деления для поиска периода дробной части
        std::string fracStr = "";
        std::map<std::string, size_t> seen_remainders; // остаток -> позиция символа

        while (!remainder.isZero()) {
            // проверка на лимит длины вывода
            if (intStr.length() + 1 + fracStr.length() >= MAX_OUTPUT_LENGTH) {
                is_trimmed = true;
                break;
            }

            std::string remKey = remainder.toString();
            if (seen_remainders.count(remKey)) {
                // период
                size_t open_bracket_pos = seen_remainders[remKey];
                std::string non_period = fracStr.substr(0, open_bracket_pos);
                std::string period = fracStr.substr(open_bracket_pos);
                return intStr + "." + non_period + "(" + period + ")";
            }

            seen_remainders[remKey] = fracStr.length();

            remainder = remainder * q;
            BigInteger nextDigit = remainder / fraction.den();
            remainder = remainder % fraction.den();

            fracStr += digitToString(std::stoi(nextDigit.toString()));
        }

        if (is_trimmed) {
            return intStr + "." + fracStr + "...";
        }
        return intStr + "." + fracStr;
    }
};
