#include "Generator.h"

namespace {
    void lower(std::string &s) {
        for (char &c: s) {
            if (c >= 'A' && c <= 'Z')
                c += 32;
        }
    }
    void upper(std::string &s) {
        for (char &c: s) {
            if (c >= 'a' && c <= 'z')
                c -= 32;
        }
    }
}


TextGenerator::TextGenerator(const std::string& filename)
    : m_out(filename)
{
    if (!of) {
        throw std::runtime_error("Cannot open file " + filename);
    }
}

void TextGenerator::printHeader(std::size_t pageNumber, std::size_t totalPages) override {
    m_out << "==== Page " << pageNumber << " out of " << totalPages << " ====\n";
}

void TextGenerator::printFooter(std::size_t pageNumber, std::size_t totalPages) override {
    m_out << "==== Page " << pageNumber << " out of " << totalPages << " ====\n";
}

void TextGenerator::printRecord(const Record& record) override {
    m_out << upper(record.key) << " : " << lower(record.value);
}

void TextGenerator::printSeparator() override {
    m_out << "-------\n";
}