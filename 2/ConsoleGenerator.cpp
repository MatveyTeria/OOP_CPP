#include "Generator.h"


void ConsoleGenerator::printRecord(const Record& record) {
    m_out << record.key << " : " << record.value << "\n";
}