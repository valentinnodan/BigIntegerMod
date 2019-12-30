#include "BigInteger.h"

template<typename T, size_t Base>
BigInteger<T, Base>::BigInteger() {
    sign = 1;
    digits.push_back(0);
}

template<typename T, size_t Base>
BigInteger<T, Base>::BigInteger(long long x) {
    if (x < 0) {
        sign = -1;
    } else {
        sign = 1;
    }
    x = std::abs(x);
    while (x > 0) {
        digits.push_back((T) (x & std::numeric_limits<T>::max()));
        x >>= (std::numeric_limits<T>::digits);
    }
}

template<typename T, size_t Base>
BigInteger<T, Base>::BigInteger(const BigInteger &other) {
    sign = other.sign;
    for (T digit: other.digits) {
        digits.push_back(digit);
    }
}

template<typename T, size_t Base>
BigInteger<T, Base>::BigInteger(BigInteger &&other) {
    digits.clear();
    sign = other.sign;
    for (T digit: other.digits) {
        digits.push_back(digit);
    }
}

template<typename T, size_t Base>
std::pair<T, bool> BigInteger<T, Base>::add_with_overflow(const T &first, const T &second) const {
    bool flag = false;
    if (std::numeric_limits<T>::max() - first < second) {
        flag = true;
    }
    return std::make_pair(first + second, flag);
}

template<typename T, size_t Base>
std::pair<T, T> BigInteger<T, Base>::mul_with_overflow(const T &first, const T &second) const {
    T b = first & ((1 << (std::numeric_limits<T>::digits >> 1)) - 1);
    T d = second & ((1 << (std::numeric_limits<T>::digits >> 1)) - 1);
    T a = first >> (std::numeric_limits<T>::digits >> 1);
    T c = second >> (std::numeric_limits<T>::digits >> 1);

    T res1 = 0;
    T res2 = 0;

    res1 += a * c;
    res2 += b * d;

    T temp1 = b * c;
    T temp2 = a * d;

    T b1 = temp1 & ((1 << (std::numeric_limits<T>::digits >> 1)) - 1);
    T d1 = temp2 & ((1 << (std::numeric_limits<T>::digits >> 1)) - 1);
    T a1 = (temp1 >> (std::numeric_limits<T>::digits >> 1));
    T c1 = temp2 >> (std::numeric_limits<T>::digits >> 1);

    std::pair<T, bool> result1 = add_with_overflow(res2, b1 << ((std::numeric_limits<T>::digits) >> 1));
    std::pair<T, bool> result2 = add_with_overflow(result1.first, d1 << ((std::numeric_limits<T>::digits) >> 1));
    T carry = (T) result1.second + (T) result2.second;
    res2 = result2.first;

    std::pair<T, bool> result3 = add_with_overflow(res1, a1);
    std::pair<T, bool> result4 = add_with_overflow(result3.first, c1);
    res1 = result4.first + carry;

    return std::make_pair(res1, res2);
}

template<typename T, size_t Base>
bool BigInteger<T, Base>::pair_compair(const std::pair<T, T> &first, const std::pair<T, T> &second) const {
    if (first.first == second.first) {
        return (first.second >= second.second);
    } else {
        return (first.first >= second.first);
    }
};

template<typename T, size_t Base>
std::pair<T, T> BigInteger<T, Base>::div_with_overflow(std::pair<T, T> pair, const T &third) const {
    T left = 0;
    T right = std::numeric_limits<T>::max();
    T middle = 0;
    while (left != right) {
        middle = ((right - left) >> 1) + left;
        std::pair<T, T> mul = mul_with_overflow(middle, third);
        bool comp = pair_compair(pair, mul);
        if (comp) {
            left = middle + 1;
        } else {
            right = middle;
        }
    }
    T answer = left - 1;
    T one;
    std::pair<T, T> ost = mul_with_overflow(answer, third);
    if (pair.second < ost.second) {
        one = pair.second + (std::numeric_limits<T>::max() - ost.second);
    } else {
        one = pair.second - ost.second;
    }


    return std::make_pair(left - 1, one);
}

template<typename T, size_t Base>
BigInteger<T, Base> &BigInteger<T, Base>::operator+=(const BigInteger<T, Base> &other) {
    BigInteger n(*this);

    if (sign == other.sign) {
        T propagate = 0;
        for (size_t i = 0; i < std::max(digits.size(), other.digits.size()); i++) {
            if (digits.size() == i) {
                digits.push_back(0);
            }
            if (other.digits.size() == i) {
                for (size_t j = i; j < digits.size(); j++) {
                    std::pair<T, bool> pair = add_with_overflow(digits[j], propagate);
                    propagate = ((T) pair.second);
                    digits[j] = pair.first;
                }
                break;
            }
            std::pair<T, bool> pair1 = add_with_overflow(other.digits[i], digits[i]);
            pair1.first += propagate;
            propagate = (pair1.second);
            digits[i] = pair1.first;
        }
        digits.push_back(propagate);
        cutBadZero();
    } else {
        if (other.sign == -1) {
            BigInteger m(other);
            m.sign = 1;
            *this = n - m;
        } else {
            BigInteger m(*this);
            m.sign = 1;
            *this = other - m;
        }
    }
    return *this;
}

template<typename T, size_t Base>
BigInteger<T, Base> &BigInteger<T, Base>::operator-=(const BigInteger<T, Base> &other) {
    BigInteger n(*this);
    if (sign == other.sign) {
        if (n >= other) {
            bool propagate = false;
            for (size_t i = 0; i < other.digits.size(); i++) {
                if (digits[i] > other.digits[i]) {
                    digits[i] -= other.digits[i];
                    digits[i] -= (T) propagate;
                    propagate = false;
                } else {
                    digits[i] = digits[i] + (std::numeric_limits<T>::max() - other.digits[i]);
                    propagate = true;
                }
            }
            if (propagate) {
                digits[other.digits.size()] += propagate;
            }
            cutBadZero();
        } else {
            BigInteger m = other - n;
            m.sign = this->sign * (-1);
            *this = m;
        }
    } else {
        if (other.sign == -1) {
            BigInteger m(other);
            m.sign = 1;
            *this = n + m;
        } else {
            BigInteger m(other);
            m.sign = -1;
            *this = n + m;
        }
    }
    return *this;
}

template<typename T, size_t Base>
BigInteger<T, Base> BigInteger<T, Base>::operator-(const BigInteger<T, Base> &other) const {
    BigInteger n(*this);
    n -= other;
    return n;
}

template<typename T, size_t Base>
void BigInteger<T, Base>::cutBadZero() {
    while ((int) digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
}

template<typename T, size_t Base>
T BigInteger<T, Base>::div_long_short(T number) {
    uint32_t propagate = 0;
    for (size_t i = (digits.size()); i > 0; i--) {
        std::pair<T, T> divide = div_with_overflow(std::make_pair(propagate, digits[i - 1]), number);
        digits[i - 1] = divide.first;
        propagate = divide.second;
    }
    cutBadZero();
    return (T) propagate;
}

template<typename T, size_t Base>
void BigInteger<T, Base>::mul_long_short(T number) {
    T propagate = 0;
    for (T &digit : digits) {
        std::pair<T, T> pair = mul_with_overflow(digit, number);
        std::pair<T, bool> result = add_with_overflow(pair.second, propagate);
        pair.first += (T) result.second;
        digit = result.first;
        propagate = pair.first;
    }
    if (propagate) {
        digits.push_back(propagate);
    }
}

template<typename T, size_t Base>
void BigInteger<T, Base>::add_long_short(T number) {
    T propagate = number;
    for (T &digit : digits) {
        std::pair<T, bool> result = add_with_overflow(digit, propagate);
        digit = result.first;
        propagate = (T) result.second;
        if (propagate == 0) {
            break;
        }
    }
    if (propagate) {
        digits.push_back(propagate);
    }
}

std::string toString(uint64_t number) {
    std::string str = "";
    if (number == 0) {
        return "0";
    }
    while (number > 0) {
        str += ((number % 10) + '0');
        number /= 10;
    }
    //reverse(str.begin(),str.end());
    return str;
}

unsigned int toInteger(char ch) {
    return (ch - '0');
}

template<typename T, size_t Base>
BigInteger<T, Base> BigInteger<T, Base>::operator+(const BigInteger<T, Base> &other) const {
    BigInteger n(*this);
    n += other;
    return n;
}

template<typename T, size_t Base>
bool BigInteger<T, Base>::compare_without_sign(const BigInteger<T, Base> &Big) {
    if (digits.size() > Big.digits.size()) {
        return true;
    } else if (digits.size() == Big.digits.size()) {
        for (int i = digits.size() - 1; i >= 0; i--) {
            if (digits[i] > Big.digits[i]) {
                return true;
            } else if (digits[i] < Big.digits[i]) {
                return false;
            }
        }
        return false;
    } else {
        return false;
    }
}

template<typename T, size_t Base>
bool BigInteger<T, Base>::compare_without_sign_and_equals(const BigInteger<T, Base> &other) {
    if (digits.size() > other.digits.size()) {
        return true;
    } else if (digits.size() == other.digits.size()) {
        for (int i = digits.size() - 1; i >= 0; i--) {
            if (digits[i] < other.digits[i]) {
                return false;
            } else if (digits[i] > other.digits[i]) {
                return true;
            }
        }
        return true;
    } else {
        return false;
    }
}

template<typename T, size_t Base>
bool BigInteger<T, Base>::operator>(const BigInteger<T, Base> &other) {
    cutBadZero();
    BigInteger Big(other);
    Big.cutBadZero();

    if (sign == Big.sign) {
        bool res = compare_without_sign(other);
        return sign == 1 == res;
    } else {
        if (digits.size() == other.digits.size() && other.digits.size() == 1
            && digits[0] == 0 && other.digits[0] == 0) {
            return false;
        }
        return sign > Big.sign;
    }
}

template<typename T, size_t Base>
BigInteger<T, Base> BigInteger<T, Base>::operator*(const BigInteger<T, Base> &other) const {
    BigInteger bigInt(0);
    bigInt.sign = sign * other.sign;
    unsigned long long result;
    int pointer = 0;
    int pointer_copy = pointer;
    for (unsigned int digit : digits) {
        unsigned long long propagate = 0;
        for (unsigned int digit1 : other.digits) {
            if (bigInt.digits.size() <= pointer) {
                bigInt.digits.push_back(0);
            }
            result = (unsigned long long) digit * (unsigned long long) digit1 + propagate +
                     (unsigned long long) bigInt.digits[pointer];
            bigInt.digits[pointer] = static_cast<unsigned int>(result & ((1LL << 33) - 1));
            propagate = result >> 32;
            pointer++;
        }
        if (bigInt.digits.size() <= pointer) {
            bigInt.digits.push_back(0);
        }
        bigInt.digits[pointer] += propagate;
        pointer = (++pointer_copy);
    }
    bigInt.cutBadZero();
    return bigInt;
}

template<typename T, size_t Base>
BigInteger<T, Base> &BigInteger<T, Base>::operator*=(const BigInteger<T, Base> &other) {
    *this = *this * other;
    return *this;
}

template<typename T, size_t Base>
bool BigInteger<T, Base>::operator==(const BigInteger<T, Base> &other) {
    if (sign != other.sign) {
        return digits.size() == other.digits.size() && other.digits.size() == 1
               && digits[0] == 0 && other.digits[0] == 0;
    }
    if (digits.size() != other.digits.size()) {
        return false;
    } else if (digits.size() == other.digits.size()) {
        for (int i = digits.size() - 1; i >= 0; i--) {
            if (digits[i] != other.digits[i]) {
                return false;
            }
        }
        return true;
    }
}

template<typename T, size_t Base>
bool BigInteger<T, Base>::operator>=(const BigInteger<T, Base> &other) {
    if (sign == other.sign) {
        bool res = compare_without_sign_and_equals(other);
        if (sign == 1) {
            return res;
        } else {
            return !res;
        }
    } else {
        if (digits.size() == other.digits.size() && other.digits.size() == 1
            && digits[0] == 0 && other.digits[0] == 0) {
            return true;
        } else {
            return sign > other.sign;
        }
    }
}

template<typename T, size_t Base>
bool BigInteger<T, Base>::operator<=(const BigInteger<T, Base> &other) {
    BigInteger n(*this);
    return !(*this > other);
}

template<typename T, size_t Base>
bool BigInteger<T, Base>::operator!=(const BigInteger<T, Base> &other) {
    return !(*this == other);
}

template<typename T, size_t Base>
bool BigInteger<T, Base>::operator<(const BigInteger<T, Base> &other) {
    return !(*this >= other);
}

template<typename T, size_t Base>
std::string toString(BigInteger<T, Base> &curr) {
    std::string out;
    if (curr.digits.empty() || (curr.digits.size() == 1 && curr.digits[0] == 0)) {
        out = "0";
    } else {
        BigInteger<T, Base> BI(curr);
        out = "";
        while (!(BI.digits.size() == 1 && BI.digits[0] == 0)) {
            T out1 = BI.div_long_short(Base);
            out += (toString(out1) + "_");
        }
        out.pop_back();
        out += (BI.sign == 1 ? "" : "-");
        reverse(out.begin(), out.end());
    }
    return out;
}

template<typename T, size_t Base>
std::ostream &operator<<(std::ostream &stream, BigInteger<T, Base> &curr) {
    stream << toString(curr);
    return stream;
}

template<typename T, size_t Base>
template<typename T1, size_t NewBase>
explicit BigInteger<T, Base>::operator BigInteger<T1, NewBase>() {
    std::string number = toString(*this);
    BigInteger<T1, NewBase> curr;
    if (number[0] == '-') {
        curr.sign = -1;
        number = number.substr(1, number.size());
    } else {
        curr.sign = 1;
    }
    for (char i : number) {
        curr.mul_long_short(NewBase);
        curr.add_long_short(toInteger(i));
    }
    curr.cutBadZero();
    return curr;
}

template<typename T, size_t Base>
size_t BigInteger<T, Base>::Size() const {
    BigInteger<T, Base> BI(*this);
    size_t size = 0;
    while (!(BI.digits.size() == 1 && BI.digits[0] == 0)) {
        BI.div_long_short(Base);
        size++;
    }
    return size;
}

template<typename T, size_t Base>
T BigInteger<T, Base>::operator[](size_t id) const {
    BigInteger<T, Base> BI(*this);
    T curr;
    for (size_t i = 0; i < BI.Size() - id; i++) {
        curr = BI.div_long_short(Base);
    }
    return curr;
}

template<typename T, size_t Base>
std::istream &operator>>(std::istream &stream, BigInteger<T, Base> &curr) {
    std::string number;
    stream >> number;
    if (number[0] == '-') {
        curr.sign = -1;
        number = number.substr(1, number.size());
    } else {
        curr.sign = 1;
    }
    for (char i : number) {
        curr.mul_long_short(Base);
        curr.add_long_short(toInteger(i));
    }
    curr.cutBadZero();
    return stream;
}
