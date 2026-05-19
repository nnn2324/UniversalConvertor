#pragma once
#include "BigInteger.h"

class BigFraction {
private:
    BigInteger num_val;
    BigInteger den_val;

    // НОД для сокращения дроби
    static BigInteger gcd(BigInteger a, BigInteger b) {
        while (!b.isZero()) {
            BigInteger temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

public:
    BigFraction(BigInteger n, BigInteger d) : num_val(n), den_val(d) {
        if (den_val.isZero()) {
            den_val = 1; // защита от деления на ноль
        }
        // сокращаем дробь
        BigInteger g = gcd(num_val, den_val);
        num_val /= g;
        den_val /= g;
    }

    BigInteger getIntegerPart() const { return num_val / den_val; }
    BigInteger getRemainder() const { return num_val % den_val; }
    BigInteger num() const { return num_val; }
    BigInteger den() const { return den_val; }
};
