#pragma once
#include "Record.h"

class ReportData
{
    private:
        std::vector<Record> records_;
    public:
        void loadFromFile(std::string filename);
        const std::vector<Record>& getRecords() const { return records_; };
        std::size_t size() const { return records_.size(); };
};
