#include "TextGenerator.h"
#include "ConsoleGenerator.h"
#include "HtmlGenerator.h"

int main() {
    try {
        ReportData data;
        data.loadFromFile("input.txt");
        ConsoleGenerator cg;
        TextGenerator tg("output.txt");
        HtmlGenerator hg("output.html");
        cg.generate(data);
        tg.generate(data);
        hg.generate(data);
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}