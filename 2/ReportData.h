#pragma once
#include "Record.h"

class ReportData
{
    std::vector<Record> records_;
    public:
        void loadFromFile(std::string filename);
        const std::vector<Record>& getRecords() const;
        std::size_t size() const;
};
