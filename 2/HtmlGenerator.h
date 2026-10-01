#pragma once
#include "Generator.h"

class HtmlGenerator: public Generator
{
public:
    explicit HtmlGenerator(const std::string& filename);

protected:
    void printRecord(const Record& record) override;
    void printSeparator() override {}
    void printHeader(std::size_t, std::size_t) override;
    void printFooter(std::size_t, std::size_t) override;
    std::size_t recordsPerPage() const override { return 20; }
    void begin() override;
    void end() override;
private:
    std::ofstream m_out;
};
