#include "word_count.h"

#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 3){
        std::cerr << "Usage: word_count.exe input.txt output.csv\n";
        return 1;
    }

    WordCounter wc;
    wc.loadFromFile(argv[1]);
    wc.countWords();
    wc.saveToCsv(argv[2]);

    std::cout << "Words: " << wc.getFrequencies().size()
                << ", total: " << wc.getTotalWords() << "\n";


    return 0;
}