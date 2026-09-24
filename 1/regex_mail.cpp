#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <cassert>

bool check_email(const std::string &s){
    std::regex self_regex(R"([a-zA-Z0-9._-]+@[a-zA-Z0-9._-]+\.[a-zA-Z0-9_-]+)");
    if (!std::regex_match(s, self_regex))
        return false;
    for (int i = 1; i < s.length(); i++){
        if (s[i] == '.' && s[i - 1] == '.')
            return false;
    }
    return true;
}

int main()
{
    assert(check_email("adf@mail.ru"));
    assert(!check_email("sdfru"));
    assert(!check_email("adf@mail"));
    std::cout << "Tests passed\n";

    std::string s;
    std::cin >> s;
    if (check_email(s))
        std::cout << "Valid email\n";
    else
        std::cout << "Not a valid email\n";
}