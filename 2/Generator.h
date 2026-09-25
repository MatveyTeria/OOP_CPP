#pragma once
#include <string>
#include <vector>

struct Record
{
    std::string key;
    std::string value;
};

class ReportData
{
    std::vector<Record> records_;
    public:
        void loadFromFile(std::string filename);
        const std::vector<Record>& getRecords() const;
        std::size_t size() const;
};

class Generator
{
    virtual void printHeader(std::size_t pageNumber, std::size_t totalPages) = 0;
    virtual void printFooter(std::size_t pageNumber, std::size_t totalPages) = 0;
    virtual void printSeparator() = 0;
    public:
        virtual void printRecord(const Record& record);
        void printPage(std::size_t start_idx, int records_on_page);
        void generate();
        virtual ~Generator();
};

class ConsoleGenerator: Generator
{
    public:
};

class TextGenerator: Generator
{
    public:
};

class HtmlGenerator: Generator
{
    public:
};
