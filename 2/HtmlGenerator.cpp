#include "Generator.h"

HtmlGenerator::HtmlGenerator(const std::string& filename)
    : m_out(filename)
{
    if (!m_out) {
        throw std::runtime_error("Cannot open file " + filename);
    }
}

void HtmlGenerator::printHeader(std::size_t pageNumber, std::size_t totalPages) {
    m_out << "<h2>Page " << pageNumber << " / " << totalPages << "</h2>\n";
    m_out << "<table border=\"1\">\n";
}

void HtmlGenerator::printFooter(std::size_t pageNumber, std::size_t totalPages) {
    m_out << "</table>\n";
}

void HtmlGenerator::printRecord(const Record& record) {
    m_out << "<tr><td><b>" << record.key << "</b></td>"
          << "<td><i>" << record.value << "</i></td></tr>\n";
}

void HtmlGenerator::begin() {
    m_out << "<!DOCTYPE html>\n<html>\n<head>\n"
          << "<meta charset=\"utf-8\">\n"
          << "<title>Report</title>\n"
          << "</head>\n<body>\n";
}

void HtmlGenerator::end() {
    m_out << "</body>\n</html>\n";
}