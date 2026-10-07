#include "Generator.h"

namespace {
    std::string lower(const std::string &s) {
        std::string res;
        res.reserve(s.size() + 1);
        for (char c: s) {
            if (c >= 'A' && c <= 'Z')
                c += 32;
            if (c != 0)
                res.push_back(c);
        }
        return res;
    }
    std::string upper(const std::string &s) {
        std::string res;
        res.reserve(s.size() + 1);
        for (char c: s) {
            if (c >= 'a' && c <= 'z')
                c -= 32;
            if (c != 0)
                res.push_back(c);
        }
        return res;
    }
}


TextGenerator::TextGenerator(const std::string& filename)
    : m_out(filename)
{
    if (!m_out) {
        throw std::runtime_error("Cannot open file " + filename);
    }
}

void TextGenerator::printHeader(std::size_t pageNumber, std::size_t totalPages) {
    m_out << "==== Page " << pageNumber << " out of " << totalPages << " ====\n";
}

void TextGenerator::printFooter(std::size_t pageNumber, std::size_t totalPages) {
    m_out << "==== Page " << pageNumber << " out of " << totalPages << " ====\n";
}

void TextGenerator::printRecord(const Record& record) {
    m_out << upper(record.key) << " : " << lower(record.value) << "\n";
}

void TextGenerator::printSeparator() {
    m_out << "-------\n";
}