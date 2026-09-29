#include "Generator.h"

namespace {

}


HtmlGenerator::HtmlGenerator(const std::string& filename)
    : m_out(filename)
{
    if (!of) {
        throw std::runtime_error("Cannot open file " + filename);
    }
}

void TextGenerator::printHeader(std::size_t pageNumber, std::size_t totalPages) override {
    m_out << "<h2>Page " << pageNumber << " / " << totalPages << "</h2>\n";
    m_out << "<table border=\"1\">\n";
}

void TextGenerator::printFooter(std::size_t pageNumber, std::size_t totalPages) override {
    m_out << "</table>\n";
}

void TextGenerator::printRecord(const Record& record) override {
    m_out << "<tr><td><b>" << record.key << "</b></td>"
          << "<td><i>" << record.value << "</i></td></tr>\n";
}

HtmlGenerator::begin() {
    m_out << "<!DOCTYPE html>\n<html>\n<head>\n"
          << "<meta charset=\"utf-8\">\n"
          << "<title>Report</title>\n"
          << "</head>\n<body>\n";
}

HtmlGenerator::end() {
    m_out << "</body>\n</html>\n";
}