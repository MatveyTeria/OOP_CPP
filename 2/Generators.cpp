class Generator
{
    bool read_input_file(const std::string &filename){
        std::ifstream in(filename);
        if (!in.is_open())
            throw std::runtime_error("Cannot open input file: " + filename);
    }
    public:
    virtual 
}

class ConsoleGenerator: Generator
{
    public:
}

class TextGenerator: Generator
{
    public:
}

class HtmlGenerator: Generator
{
    public:
}