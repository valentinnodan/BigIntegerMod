#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <iomanip>

template<typename T, size_t Base>
class BigInteger;

template<typename T, size_t Base>
std::ostream &operator<<(std::ostream &, BigInteger<T, Base> &);

template<typename T, size_t Base>
std::ostream &operator<<(std::ostream &, BigInteger<T, Base> &);

template<typename T, size_t Base = std::numeric_limits<T>::max()>
class BigInteger {

    friend std::ostream &operator<<<>(std::ostream &, BigInteger &);

    friend std::ostream &operator<<<>(std::ostream &, BigInteger &);


public:
    T operator[](size_t id) const;
    template<typename T1, size_t NewBase>
    explicit operator BigInteger<T1, NewBase>();
    void cutBadZero();

    std::pair<T, bool> add_with_overflow(const T &first, const T &second) const;

    std::pair<T, T> mul_with_overflow(const T &first, const T &second) const;

    std::pair<T, T> div_with_overflow(std::pair<T, T> pair, const T &third) const;

    short compare(BigInteger<T, Base> &other);

    BigInteger<T, Base> multiply(const BigInteger<T, Base> &first ,const BigInteger<T, Base> &second);

    T div_long_short(T number);

    void mul_long_short(T number);

    void add_long_short(T number);

    int sign;

    BigInteger();

    explicit BigInteger(long long x);

    std::vector<T> digits;

    //explicit BigInteger(long long value = 0);

    BigInteger(const BigInteger &);

    BigInteger &operator=(const BigInteger &) = default;

    BigInteger(BigInteger &&);

    BigInteger &operator+=(const BigInteger &);

    BigInteger &operator*=(const BigInteger &);

    BigInteger operator+(const BigInteger &) const;

    BigInteger operator*(const BigInteger &) const;

    BigInteger &operator-=(const BigInteger &);

    BigInteger operator-(const BigInteger &) const;

    bool operator>(const BigInteger &);

    bool operator==(const BigInteger &);

    bool operator>=(const BigInteger &);

    bool operator<(const BigInteger &);

    bool operator!=(const BigInteger &);

    bool operator<=(const BigInteger &);

    size_t Size() const;

    bool pair_compair(const std::pair<T, T> &first, const std::pair<T, T> &second) const;

};
