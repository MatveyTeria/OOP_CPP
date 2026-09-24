#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <cassert>
#include <vector>

bool check_brackets(const std::string &phone_number){
    int open_bracket_pos = -1, closed_bracket_pos = -1;
    for (int i = 0; i < phone_number.length(); i++){
        switch (phone_number[i])
        {
        case '(':
            if (open_bracket_pos != -1){
                return false;
            }
            open_bracket_pos = i;
            break;
        case ')':
            if (closed_bracket_pos != -1){
                return false;
            }
            closed_bracket_pos = i;
            break;
        
        default:
            break;
        }
    }
    if (open_bracket_pos > closed_bracket_pos)
        return false;
    return true;
}

bool check_dashes(const std::string &s){
    for (int i = 1; i < s.length(); i++){
        if (s[i] == '-' && s[i - 1] == '-'){
            return false;
        }
        if (i == s.length() - 1 && s[i] == '-'){
            return false;
        }
    }
    return true;
}

std::string finalize_phone_number(const std::string &s){
    std::string result;
    result.reserve(15);
    result += "+7";
    bool first_digit = true;
    for (int i = 0; i < s.length(); i++){
        if (s[i] > '9' | s[i] < '0')
            continue;
        
        if (first_digit){
            first_digit = false;
            continue;
        }
        result.push_back(s[i]);
    }
    return result;
}

std::string check_and_format_phone_number(const std::string &s){
    std::regex self_regex(R"((\+7|8)[\d\-\(\) ]+)");
    if (!std::regex_match(s, self_regex))
        return "error";

    if (!check_brackets(s))
        return "error";
    
    if (!check_dashes(s))
        return "error";
    
    std::string result = finalize_phone_number(s);
    if (result.length() != 12)
        return "error";
    
    return result;
}

int main()
{
    assert(check_and_format_phone_number("+7(902)123-4567") == "+79021234567");
    assert(check_and_format_phone_number("8(902)1-2-3-45-67") == "+79021234567");
    assert(check_and_format_phone_number("504)635(2299") == "error");
    assert(check_and_format_phone_number("8--9019876543-22-3—4") == "error");
    std::cout << "Tests passed\n";
    
    std::string s;
    std::getline(std::cin, s);
    std::string result = check_and_format_phone_number(s);
   
    std::cout << result << '\n';
}