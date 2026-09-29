#include "Generator.h"


ConsoleGenerator::printRecord(const Record& record) override {
    m_out << record.key << " : " << record.value;
}