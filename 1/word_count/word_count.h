#pragma once

#include <string>
#include <list>
#include <map>

// Класс для подсчёта частоты слов в текстовом файле
class WordCounter
{
public:
    // Загрузить строки из файла в std::list
    void loadFromFile(const std::string& filename);

    // Разбить строки на слова и посчитать частоты
    void countWords();

    // Сохранить результат в CSV файл
    void saveToCsv(const std::string& filename) const;

    const std::list<std::string>& getLines() const { return m_lines; }
    const std::map<std::string, size_t>& getFrequencies() const { return m_freq; }
    size_t getTotalWords() const { return m_totalWords; }

private:
    std::list<std::string> m_lines;
    std::map<std::string, size_t> m_freq;
    size_t m_totalWords = 0;
};