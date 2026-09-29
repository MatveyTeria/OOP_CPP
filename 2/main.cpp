#include "Generator.h"

int main() {
    ReportData data;
    data.loadFromFile("input.txt");
    ConsoleGenerator cg;
    TextGenerator tg;
    HtmlGenerator hg;
    cg.generate(data);
    tg.generate(data);
    hg.generate(data);
}