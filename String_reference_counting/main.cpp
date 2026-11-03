#include "String.hpp"
#include <iostream>

int main() {
    String empty;
    String a("abebe");
    String b(12, 'c');
    a += 'b';
    String c(a);
    String d("frf");
    empty += "auvaufrfrfr";
    std::cout << empty.find(d) << '\n';
    std::cout << empty.data() << '\n';
    std::cout << a.data() << '\n';
    std::cout << b.data() << '\n';
    std::cout << c.data() << '\n';
    return 0;
}