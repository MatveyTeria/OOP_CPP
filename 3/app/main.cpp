#include "Generator.h"
#include <iostream>

int main() {
    try{
        ReportData data;
        data.loadFromFile("input.txt");
        ConsoleGenerator cg;
        TextGenerator tg;
        HtmlGenerator hg;
        cg.generate(data);
        tg.generate(data);
        hg.generate(data);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}