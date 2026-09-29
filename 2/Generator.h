#pragma once
#include "ReportData.h"

class Generator
{
public:
    void generate(const ReportData& data);
    virtual ~Generator() = default;

    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&&) = delete;
    Generator& operator=(Generator&&) = delete;
protected:
    virtual void begin() {};
    virtual void end() {};
    Generator() = default;
    virtual void printRecord(const Record& record);
    virtual void printHeader(std::size_t pageNumber, std::size_t totalPages) = 0;
    virtual void printFooter(std::size_t pageNumber, std::size_t totalPages) = 0;
    virtual void printSeparator() = 0;
    virtual std::size_t recordsPerPage() const = 0;
private:
    void printPage(const std::vector<Record>& records,
                std::size_t startIdx,
                std::size_t count,
                std::size_t pageNumber,
                std::size_t totalPages);

    
};

class ConsoleGenerator: Generator
{
public:
    explicit ConsoleGen(std::ostream& out = std::cout) : m_out(out) {}

protected:
    void printRecord(const Record& record) override;
    void printSeparator() override {}
    void printHeader(std::size_t, std::size_t) override {}
    void printFooter(std::size_t, std::size_t) override {}
    std::size_t recordsPerPage() const override { return 20; }

private:
    std::ostream& m_out;
};

class TextGenerator: Generator
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
    std::ostream& m_out;
};

class HtmlGenerator: Generator
{
public:
    explicit HtmlGenerator(const std::string& filename);

protected:
    void printRecord(const Record& record) override;
    void printSeparator() override {}
    void printHeader(std::size_t, std::size_t) override;
    void printFooter(std::size_t, std::size_t) override;
    std::size_t recordsPerPage() const override { return 20; }

private:
    std::ostream& m_out;
};
