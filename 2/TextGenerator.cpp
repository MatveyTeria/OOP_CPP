#include "Generator.h"


ConsoleGenerator::printRecord(const Record& record) {
    m_out << record.key << " : " << record.value;
}