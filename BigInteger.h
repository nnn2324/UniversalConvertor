#ifndef BIGINTEGER_H
#define BIGINTEGER_H

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

class BigInteger {
private:
    std::vector<int> digits;
    static const int BASE = 1000000000;

    void remove_leading_zeros() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
    }

    // Вспомогательный метод для Карацубы: сдвиг числа на k разрядов (умножение на BASE^k)
    BigInteger shift_left(size_t k) const {
        if (this->isZero()) return *this;
        BigInteger res = *this;
        res.digits.insert(res.digits.begin(), k, 0);
        return res;
    }

public:
    // Изолированный статический метод Карацубы, работающий на объектах BigInteger
    static BigInteger karatsuba(const BigInteger& a, const BigInteger& b) {
        if (a.isZero() || b.isZero()) return BigInteger(0);

        size_t n = std::max(a.digits.size(), b.digits.size());

        // Если числа маленькие (<= 64 разрядов), внутри Карацубы считаем столбиком
        if (n <= 64) {
            BigInteger result;
            result.digits.resize(a.digits.size() + b.digits.size(), 0);

            for (size_t i = 0; i < a.digits.size(); ++i) {
                long long carry = 0;
                for (size_t j = 0; j < b.digits.size() || carry > 0; ++j) {
                    long long cur = result.digits[i + j] +
                                    (long long)a.digits[i] * (j < b.digits.size() ? b.digits[j] : 0) +
                                    carry;
                    result.digits[i + j] = cur % BASE;
                    carry = cur / BASE;
                }
            }
            result.remove_leading_zeros();
            return result;
        }

        size_t k = n / 2;

        // Режем объекты BigInteger на половины через строки
        std::string str_a = a.toString();
        std::string str_b = b.toString();
        size_t char_shift = k * 9;

        BigInteger a_high = 0, a_low = 0;
        if (str_a.length() > char_shift) {
            a_high = BigInteger(str_a.substr(0, str_a.length() - char_shift));
            a_low = BigInteger(str_a.substr(str_a.length() - char_shift));
        } else {
            a_low = a;
        }

        BigInteger b_high = 0, b_low = 0;
        if (str_b.length() > char_shift) {
            b_high = BigInteger(str_b.substr(0, str_b.length() - char_shift));
            b_low = BigInteger(str_b.substr(str_b.length() - char_shift));
        } else {
            b_low = b;
        }

        // Рекурсивные вызовы Карацубы
        BigInteger p1 = karatsuba(a_low, b_low);
        BigInteger p2 = karatsuba(a_high, b_high);
        BigInteger p3 = karatsuba(a_low + a_high, b_low + b_high);

        BigInteger mid = p3 - p1 - p2;

        return p2.shift_left(2 * k) + mid.shift_left(k) + p1;
    }

    // Проверка на ноль
    bool isZero() const {
        return digits.size() == 1 && digits[0] == 0;
    }

    // Конструкторы
    BigInteger() {
        digits = {0};
    }

    BigInteger(long long val) {
        if (val < 0) val = 0;
        if (val == 0) {
            digits = {0};
        } else {
            while (val > 0) {
                digits.push_back(val % BASE);
                val /= BASE;
            }
        }
    }

    BigInteger(std::string s) {
        if (s.empty() || s == "0") {
            digits = {0};
            return;
        }
        for (int i = (int)s.length(); i > 0; i -= 9) {
            if (i < 9) {
                digits.push_back(std::stoi(s.substr(0, i)));
            } else {
                digits.push_back(std::stoi(s.substr(i - 9, 9)));
            }
        }
        remove_leading_zeros();
    }

    std::string toString() const {
        if (digits.empty()) return "0";
        std::string s = std::to_string(digits.back());
        for (int i = (int)digits.size() - 2; i >= 0; --i) {
            std::string chunk = std::to_string(digits[i]);
            s += std::string(9 - chunk.length(), '0') + chunk;
        }
        return s;
    }

    // Операторы сравнения
    bool operator<(const BigInteger& other) const {
        if (digits.size() != other.digits.size()) {
            return digits.size() < other.digits.size();
        }
        for (int i = (int)digits.size() - 1; i >= 0; --i) {
            if (digits[i] != other.digits[i]) {
                return digits[i] < other.digits[i];
            }
        }
        return false;
    }

    bool operator==(const BigInteger& other) const {
        return digits == other.digits;
    }

    bool operator!=(const BigInteger& other) const { return !(*this == other); }
    bool operator>(const BigInteger& other) const { return other < *this; }
    bool operator<=(const BigInteger& other) const { return !(*this > other); }
    bool operator>=(const BigInteger& other) const { return !(*this < other); }

    // Сложение и вычитание
    BigInteger operator+(const BigInteger& other) const {
        BigInteger res;
        res.digits.clear();
        long long carry = 0;
        size_t n = std::max(digits.size(), other.digits.size());
        for (size_t i = 0; i < n || carry > 0; ++i) {
            long long cur = carry;
            if (i < digits.size()) cur += digits[i];
            if (i < other.digits.size()) cur += other.digits[i];
            res.digits.push_back(cur % BASE);
            carry = cur / BASE;
        }
        return res;
    }

    BigInteger operator-(const BigInteger& other) const {
        BigInteger res;
        res.digits.clear();
        long long borrow = 0;
        for (size_t i = 0; i < digits.size(); ++i) {
            long long other_digit = (i < other.digits.size()) ? other.digits[i] : 0;
            long long cur = digits[i] - other_digit - borrow;
            if (cur < 0) {
                cur += BASE;
                borrow = 1;
            } else {
                borrow = 0;
            }
            res.digits.push_back(cur);
        }
        res.remove_leading_zeros();
        return res;
    }

    // умножение
    BigInteger operator*(const BigInteger& other) const {
        if (this->isZero() || other.isZero()) return BigInteger(0);

        BigInteger result;
        result.digits.resize(digits.size() + other.digits.size(), 0);

        for (size_t i = 0; i < digits.size(); ++i) {
            long long carry = 0;
            for (size_t j = 0; j < other.digits.size() || carry > 0; ++j) {
                long long cur = result.digits[i + j] +
                                (long long)digits[i] * (j < other.digits.size() ? other.digits[j] : 0) +
                                carry;
                result.digits[i + j] = cur % BASE;
                carry = cur / BASE;
            }
        }

        result.remove_leading_zeros();
        return result;
    }

    // Деление
    std::pair<BigInteger, BigInteger> divmod(const BigInteger& A, const BigInteger& B) const {
        if (B.isZero()) throw std::invalid_argument("Division by zero");
        if (A < B) return {BigInteger(0), A};

        BigInteger quotient;
        quotient.digits.resize(A.digits.size(), 0);
        BigInteger current = 0;

        for (int i = (int)A.digits.size() - 1; i >= 0; --i) {
            if (current.isZero()) {
                current.digits[0] = A.digits[i];
            } else {
                current.digits.insert(current.digits.begin(), A.digits[i]);
            }
            current.remove_leading_zeros();

            long long low = 0, high = BASE - 1, ans = 0;
            while (low <= high) {
                long long mid = low + (high - low) / 2;
                if (B * mid <= current) {
                    ans = mid;
                    low = mid + 1;
                } else {
                    high = mid - 1;
                }
            }
            quotient.digits[i] = ans;
            current = current - B * ans;
        }
        quotient.remove_leading_zeros();
        return {quotient, current};
    }

    BigInteger operator/(const BigInteger& other) const { return divmod(*this, other).first; }
    BigInteger operator%(const BigInteger& other) const { return divmod(*this, other).second; }

    BigInteger operator/(long long short_val) const { return *this / BigInteger(short_val); }
    BigInteger operator%(long long short_val) const { return *this % BigInteger(short_val); }

    // Сокращенные операторы присваивания
    BigInteger& operator+=(const BigInteger& other) { return *this = *this + other; }
    BigInteger& operator-=(const BigInteger& other) { return *this = *this - other; }
    BigInteger& operator*=(const BigInteger& other) { return *this = *this * other; }
    BigInteger& operator/=(const BigInteger& other) { return *this = *this / other; }
    BigInteger& operator%=(const BigInteger& other) { return *this = *this % other; }

    friend std::ostream& operator<<(std::ostream& out, const BigInteger& val) {
        out << val.toString();
        return out;
    }
};

#endif // BIGINTEGER_H
