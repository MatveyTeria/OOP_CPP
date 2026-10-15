#include <gtest/gtest.h>
#include "mylib/point.h"
#include "mylib/color.h"
#include "mylib/image.h"
#include "mylib/bresenham.h"

TEST(BresenhemTest, TrivialCase) {
    mylib::Image img(10, 10);
    drawLine(img, mylib::Point{1, 1}, mylib::Point{1, 1}, mylib::Color{0, 0, 0});
    EXPECT_EQ(img.getPixel(mylib::Point{1, 1}), (mylib::Color{0, 0, 0}));
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            if (i == 1 && j == 1) continue;
            EXPECT_EQ(img.getPixel(mylib::Point{i, j}), (mylib::Color{255, 255, 255}));
        }
    }
}

TEST(BresenhemTest, HorizontalLine) {
    mylib::Image img(10, 10);
    drawLine(img, mylib::Point{1, 1}, mylib::Point{5, 1}, mylib::Color{0, 0, 0});
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            if (j == 1 && 1 <= i && i <= 5){
                EXPECT_EQ(img.getPixel(mylib::Point{i, j}), (mylib::Color{0, 0, 0}));
                continue;
            }
            EXPECT_EQ(img.getPixel(mylib::Point{i, j}), (mylib::Color{255, 255, 255}));
        }
    }
}

TEST(BresenhemTest, VerticalLine) {
    mylib::Image img(10, 10);
    drawLine(img, mylib::Point{1, 1}, mylib::Point{1, 5}, mylib::Color{0, 0, 0});
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            if (i == 1 && 1 <= j && j <= 5){
                EXPECT_EQ(img.getPixel(mylib::Point{i, j}), (mylib::Color{0, 0, 0}));
                continue;
            }
            EXPECT_EQ(img.getPixel(mylib::Point{i, j}), (mylib::Color{255, 255, 255}));
        }
    }
}

TEST(BresenhemTest, NotSteepLine) {
    mylib::Image img(10, 10);
    drawLine(img, mylib::Point{1, 1}, mylib::Point{2, 4}, mylib::Color{0, 0, 0});
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            if ((i == 1 && 1 <= j && j <= 2) || (i == 2 && 3 <= j && j <= 4)){
                EXPECT_EQ(img.getPixel(mylib::Point{i, j}), (mylib::Color{0, 0, 0}));
                continue;
            }
            EXPECT_EQ(img.getPixel(mylib::Point{i, j}), (mylib::Color{255, 255, 255}));
        }
    }
}

TEST(BresenhemTest, SteepLine) {
    mylib::Image img(10, 10);
    drawLine(img, mylib::Point{1, 1}, mylib::Point{4, 2}, mylib::Color{0, 0, 0});
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            if ((j == 1 && 1 <= i && i <= 2) || (j == 2 && 3 <= i && i <= 4)){
                EXPECT_EQ(img.getPixel(mylib::Point{i, j}), (mylib::Color{0, 0, 0}));
                continue;
            }
            EXPECT_EQ(img.getPixel(mylib::Point{i, j}), (mylib::Color{255, 255, 255}));
        }
    }
}