#include "BigInteger.cpp"
#include <iostream>

int main() {
    BigInteger<uint32_t, 10> x;
    std::cin >> x;
    std::cout << x << std::endl;
    BigInteger<uint64_t ,2> y = (BigInteger<uint64_t, 2>) x ;
    std::cout << y << std::endl;
    return 0;
}
