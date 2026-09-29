#include "Generator.h"

void Generator::generate(const ReportData& data) {
    const auto records = data.getRecords();
    std::size_t recordsCount = records.size();
    std::size_t perPage = recordsPerPage();
    if (perPage <= 0) {
        throw std::logic_error("recordsPerPage returned value <= 0");
    }

    std::size_t totalPages = (recordsCount + perPage - 1) / perPage; // ceil(recordsCount / perPage)

    for (std::size_t pageNumber = 0; pageNumber < totalPages; pageNumber++) {
        const std::size_t start = pageNumber * perPage;
        const std::size_t count = std::min(perPage, recordsCount - start);
        printPage(records, start, count, pageNumber + 1, totalPages);
    }
}

void Generator::printPage(
    const std::vector<Record>& records,
    std::size_t startIdx,
    std::size_t count,
    std::size_t pageNumber,
    std::size_t totalPages) {
    printHeader(pageNumber, totalPages);
    for (std::size_t i = startIdx; i < startIdx + count; i++){
        printRecord(records[i]);
        if (i < startIdx + count - 1)
            printSeparator();
    }
    printFooter(pageNumber, totalPages);
}