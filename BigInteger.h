#ifndef BIGINTEGER_H
#define BIGINTEGER_H

#endif // BIGINTEGER_H

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

    void karatsuba(int *a, int *b, int *c, int n) {
        if (n <= 64) {
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    c[i + j] += a[i] * b[j];
        } else {
            int k = n / 2;
            // динамическая память, чтобы не перегружать стек на больших числах
            int* l = new int[k];
            int* r = new int[k];
            int* t = new int[n]();

            for (int i = 0; i < k; i++) {
                l[i] = a[i] + a[k + i];
                r[i] = b[i] + b[k + i];
            }

            karatsuba(l, r, t, k); // считает t
            karatsuba(a, b, c, k); // считает p1
            karatsuba(a + k, b + k, c + n, k); // считает p2

            int *t1 = t, *t2 = t + k;
            int *s1 = c, *s2 = c + k, *s3 = c + 2 * k, *s4 = c + 3 * k;

            for (int i = 0; i < k; i++) {
                int c1 = s2[i] + t1[i] - s1[i] - s3[i];
                int c2 = s3[i] + t2[i] - s2[i] - s4[i];
                c[k + i] = c1;
                c[n + i] = c2;
            }

            delete[] l;
            delete[] r;
            delete[] t;
        }
    }
public:
    BigInteger operator*(const BigInteger& other) const {
        if (this -> isZero() || other.isZero()) return BigInteger(0);

        size_t max_len = std::max(this->digits.size(), other.digits.size());

        size_t n = 1;
        while (n < max_len) n*=2; //округляем вверх до ближайшей степени двойки

        // временые массивы нужной длины, забиты нулями
        int* a_arr = new int[n]();
        int* b_arr = new int[n]();
        int* c_arr = new int[2 * n](); // результат мин в 2 раза длинее

        // копируем из digits в массивы
        for (size_t i = 0; i < this->digits.size(); ++i) a_arr[i] = this->digits[i];
        for (size_t i = 0; i < other.digits.size(); ++i) b_arr[i] = other.digits[i];

        // карацуба
        karatsuba(a_arr, b_arr, c_arr, n);

        // внутри алгоса были вычитания, там могут быть отриц и тд
        BigInteger result;
        result.digits.clear(); //очищаем

        long long carry = 0;
        for (size_t i = 0; i < 2 * n; ++i) {
            long long cur = c_arr[i] + carry;
            if (cur < 0) {
                // если отриц занимаем у старшего разряда
                long long borrow = (-cur + 9) / 10;
                carry = -borrow;
                cur += borrow * 10;
            } else {
                carry = cur / 10;
                cur %= 10;
            }
            result.digits.push_back(cur);
        }

        // если после всего цикла остался перенос, то добавляем его в конец
        while (carry > 0) {
            result.digits.push_back(carry % 10);
            carry /= 10;
        }

        // чистим дин память за собой
        delete[] a_arr;
        delete[] b_arr;
        delete[] c_arr;

        // убираем ведущие нули, если они образовались
        result.remove_leading_zeros();

        return result;
    }
    // конструкторы
    BigInteger() {
        digits = {0};
    }

    BigInteger(long long val) {
        if (val < 0) val = 0; // по условию только неотриц
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

    // проверка на ноль и преобразование в строку
    bool isZero() const {
        return digits.size() == 1 && digits[0] == 0;
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

    // операторы сравнения
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



    // деление
    std::pair<BigInteger, BigInteger> divmod(const BigInteger& A, const BigInteger& B) {
        if (B.isZero()) throw std::invalid_argument("Division by zero");
        if (A < B) return {BigInteger(0), A};

        BigInteger quotient;
        quotient.digits.resize(A.digits.size(), 0);
        BigInteger current = 0;

        // деление столбиком
        for (int i = (int)A.digits.size() - 1; i >= 0; --i) {
            if (current.isZero()) {
                current.digits[0] = A.digits[i];
            } else {
                current.digits.insert(current.digits.begin(), A.digits[i]);
            }
            current.remove_leading_zeros();

            // бин поиск цифры частного в диапазоне от 0 до base-1
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

    // деление на маленькое число
    BigInteger operator/(long long short_val) const { return *this / BigInteger(short_val); }
    BigInteger operator%(long long short_val) const { return *this % BigInteger(short_val); }

    // сокращенные операторы присваивания
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
