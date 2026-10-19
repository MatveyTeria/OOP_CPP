#include "TextGenerator.h"
#include "ConsoleGenerator.h"
#include "HtmlGenerator.h"

#include <iostream>
#include <memory>
#include <string>

namespace {

void printUsage(const char* programName)
{
    std::cerr
        << "Usage:\n"
        << "  " << programName << " console <input.txt>\n"
        << "  " << programName << " text    <input.txt> <output.txt>\n"
        << "  " << programName << " html    <input.txt> <output.html>\n"
        << "\n"
        << "Generators:\n"
        << "  console  — print report to stdout\n"
        << "  text     — write plain-text report to file\n"
        << "  html     — write HTML report to file\n";
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    const std::string generator = argv[1];
    const std::string inputFile = argv[2];

    try {
        ReportData data;
        data.loadFromFile(inputFile);

        std::unique_ptr<Generator> gen;

        if (generator == "console") {
            gen = std::make_unique<ConsoleGenerator>();
        }
        else if (generator == "text") {
            if (argc < 4) {
                std::cerr << "Error: 'text' generator requires output file\n\n";
                printUsage(argv[0]);
                return 1;
            }
            gen = std::make_unique<TextGenerator>(argv[3]);
        }
        else if (generator == "html") {
            if (argc < 4) {
                std::cerr << "Error: 'html' generator requires output file\n\n";
                printUsage(argv[0]);
                return 1;
            }
            gen = std::make_unique<HtmlGenerator>(argv[3]);
        }
        else {
            std::cerr << "Error: unknown generator '" << generator << "'\n\n";
            printUsage(argv[0]);
            return 1;
        }

        gen->generate(data);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}