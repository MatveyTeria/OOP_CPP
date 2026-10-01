#pragma once
#include "Generator.h"

class ConsoleGenerator: public Generator
{
public:
    explicit ConsoleGenerator(std::ostream& out = std::cout) : m_out(out) {}

protected:
    void printRecord(const Record& record) override;
    void printSeparator() override {}
    void printHeader(std::size_t, std::size_t) override {}
    void printFooter(std::size_t, std::size_t) override {}
    std::size_t recordsPerPage() const override { return 20; }

private:
    std::ostream& m_out;
};