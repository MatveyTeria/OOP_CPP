#include "TextGenerator.h"
#include "ConsoleGenerator.h"
#include "HtmlGenerator.h"

int main() {
    ReportData data;
    data.loadFromFile("input.txt");
    ConsoleGenerator cg;
    TextGenerator tg("output.txt");
    HtmlGenerator hg("output.html");
    cg.generate(data);
    tg.generate(data);
    hg.generate(data);
}