#pragma once
#include "BigFraction.h"
#include <string>
#include <vector>
#include <stdexcept>

class BaseParser {
private:
    // парсит строку в массив отдельных цифр с учетом квадратных скобок
    static std::vector<int> parseDigits(const std::string& s, int p) {
        std::vector<int> digits;
        size_t i = 0;
        while (i < s.length()) {
            if (s[i] == '[') {
                i++;
                std::string num = "";
                while (i < s.length() && s[i] != ']') {
                    num += s[i];
                    i++;
                }
                if (i >= s.length()) throw std::invalid_argument("Незакрытая квадратная скобка в числе");
                i++; // Пропускаем ']'
                if (num.empty()) throw std::invalid_argument("Пустые квадратные скобки []");

                int val = std::stoi(num);
                if (val < 0 || val >= p) {
                    throw std::invalid_argument("Цифра [" + num + "] выходит за пределы основания " + std::to_string(p));
                }
                digits.push_back(val);
            } else {
                char c = s[i];
                int val = -1;
                if (c >= '0' && c <= '9') val = c - '0';
                else if (c >= 'A' && c <= 'Z') val = c - 'A' + 10;
                else if (c >= 'a' && c <= 'z') val = c - 'a' + 10;

                if (val == -1 || val >= p) {
                    throw std::invalid_argument(std::string("Недопустимый символ '") + c + "' для основания " + std::to_string(p));
                }
                digits.push_back(val);
                i++;
            }
        }
        return digits;
    }

    static BigInteger digitsToBigInt(const std::vector<int>& digits, int p) {
        BigInteger res(0);
        for (int d : digits) {
            res = res * p + BigInteger(d);
        }
        return res;
    }

public:
    static BigFraction parse(const std::string& input, int p) {
        if (input.empty()) throw std::invalid_argument("Пустая строка ввода");
        if (!input.empty() && input[0] == '.') {
            throw std::invalid_argument("Ошибка ввода: отсутствует целая часть перед точкой!");
        }

        size_t dot_pos = input.find('.');
        size_t paren_pos = input.find('(');

        std::string I_str = "";
        std::string F_str = "";
        std::string P_str = "";


        // рзделяем строку на составные компоненты
        if (dot_pos == std::string::npos) {
            if (paren_pos != std::string::npos) throw std::invalid_argument("Период не может идти без дробной точки");
            I_str = input;
        } else {
            I_str = input.substr(0, dot_pos);
            if (paren_pos == std::string::npos) {
                F_str = input.substr(dot_pos + 1);
            } else {
                if (paren_pos < dot_pos) throw std::invalid_argument("Неверный порядок точеки и скобок");
                F_str = input.substr(dot_pos + 1, paren_pos - (dot_pos + 1));
                size_t close_paren = input.find(')', paren_pos);
                if (close_paren == std::string::npos) throw std::invalid_argument("Пропущена закрывающая скобка периода");
                if (close_paren-paren_pos == 1) throw std::invalid_argument("Пустой период");
                P_str = input.substr(paren_pos + 1, close_paren - (paren_pos + 1));
            }
        }

        if (F_str.empty() && P_str.empty() && dot_pos != std::string::npos) {
            throw std::invalid_argument("Ошибка ввода: после точки должна идти дробная часть или период!");
        }

        std::vector<int> I_digits = parseDigits(I_str, p);
        std::vector<int> F_digits = F_str.empty() ? std::vector<int>{} : parseDigits(F_str, p);
        std::vector<int> P_digits = P_str.empty() ? std::vector<int>{} : parseDigits(P_str, p);

        BigInteger I = digitsToBigInt(I_digits, p);
        BigInteger F = digitsToBigInt(F_digits, p);
        BigInteger P = digitsToBigInt(P_digits, p);

        // считаем степени p^k и p^m
        BigInteger pk(1);
        for (size_t i = 0; i < F_digits.size(); ++i) pk = pk * p;

        BigInteger pm(1);
        for (size_t i = 0; i < P_digits.size(); ++i) pm = pm * p;

        // применяем мат формулу свертки в обыкновенную дробь
        if (P_digits.empty()) {
            return BigFraction(I * pk + F, pk);
        } else {
            BigInteger pm_minus_1 = pm - BigInteger(1);
            BigInteger den = pk * pm_minus_1;
            BigInteger num = I * den + F * pm_minus_1 + P;
            return BigFraction(num, den);
        }
    }
};
