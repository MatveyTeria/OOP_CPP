#include "word_count.h"

#include <fstream>
#include <stdexcept>
#include <cctype>
#include <vector>
#include <algorithm>

void WordCounter::loadFromFile(const std::string& filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
        throw std::runtime_error("Cannot open input file: " + filename);

    m_lines.clear();
    std::string line;
    while (std::getline(in, line))
        m_lines.push_back(line);
}

void WordCounter::countWords()
{
    m_freq.clear();
    m_totalWords = 0;

    for (const std::string& line : m_lines)
    {
        std::string word;
        for (char ch : line)
        {
            unsigned char uc = static_cast<unsigned char>(ch);
            if (std::isalnum(uc))
                word += static_cast<char>(std::tolower(uc));
            else if (!word.empty())
            {
                ++m_freq[word];
                ++m_totalWords;
                word.clear();
            }
        }
        if (!word.empty())
        {
            ++m_freq[word];
            ++m_totalWords;
            word.clear();
        }
    }
}

void WordCounter::saveToCsv(const std::string& filename) const
{
    std::ofstream out(filename);
    if (!out.is_open())
        throw std::runtime_error("Cannot open output file: " + filename);

    std::vector<std::pair<std::string, size_t>> items(m_freq.begin(), m_freq.end());

    std::sort(items.begin(), items.end(),
        [](const auto& a, const auto& b)
        {
            if (a.second != b.second)
                return a.second > b.second;
            return a.first < b.first;
        });

    out << "Word,Frequency,Percent\n";
    for (const auto& [word, count] : items)
    {
        double percent = (m_totalWords == 0)
                       ? 0.0
                       : 100.0 * static_cast<double>(count) / static_cast<double>(m_totalWords);

        out << word << ',' << count << ',' << percent << '\n';
    }
}