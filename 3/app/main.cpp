#include "mylib/point.h"
#include "mylib/color.h"
#include "mylib/image.h"
#include "mylib/bresenham.h"
#include "mylib/parser.h"

#include <fstream>
#include <iostream>
#include <exception>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "usage: app <task-file>\n";
        return 1;
    }

    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "cannot open task file: " << argv[1] << "\n";
        return 1;
    }

    try {
        mylib::Parser parser;
        auto req = parser.parse(in);
        if (!req) {
            std::cerr << "invalid task file\n";
            return 1;
        }
        std::cout << "File parsed\n";

        auto img = mylib::Image::loadBmp(req->inputPath);
        std::cout << "Image loaded\n";
        mylib::drawLine(img, req->start, req->end, req->color);
        std::cout << "Line drawn\n";
        img.saveBmp(req->outputPath);
        std::cout << "Image saved\n";
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}