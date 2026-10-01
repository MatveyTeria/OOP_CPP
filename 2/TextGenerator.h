#pragma once
#include "Generator.h"

class TextGenerator: public Generator
{
public:
    explicit TextGenerator(const std::string& filename);

protected:
    void printRecord(const Record& record) override;
    void printSeparator() override;
    void printHeader(std::size_t, std::size_t) override;
    void printFooter(std::size_t, std::size_t) override;
    std::size_t recordsPerPage() const override { return 20; }

private:
    std::ofstream m_out;
};