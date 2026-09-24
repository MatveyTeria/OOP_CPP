#include <iostream>
#include <regex>
#include <string>

int main() {
    std::regex self_regex("a");
    std::string s = "abebe@gmail.com";
    if (std::regex_match(s, self_regex))
        std::cout << "YES";
    else
        std::cout << "NO";
    return 0;
}