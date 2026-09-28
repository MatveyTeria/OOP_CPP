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
    public:
        void generate(const ReportData& data);
        virtual ~Generator() = default;

        Generator(const Generator&) = delete;
        Generator& operator=(const Generator&) = delete;
        Generator(Generator&&) = delete;
        Generator& operator=(Generator&&) = delete;
    protected:
        Generator() = default;
        virtual void printRecord(const Record& record);
        virtual void printHeader(std::size_t pageNumber, std::size_t totalPages) = 0;
        virtual void printFooter(std::size_t pageNumber, std::size_t totalPages) = 0;
        virtual void printSeparator() = 0;
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
};

class TextGenerator: Generator
{
    public:
};

class HtmlGenerator: Generator
{
    public:
};
