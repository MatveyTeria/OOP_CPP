#include <gtest/gtest.h>
#include "mylib/point.h"
#include "mylib/color.h"
#include "mylib/image.h"

TEST(ImageTest, getAndSetPixel) {
    mylib::Image img(10, 10);
    img.setPixel(mylib::Point{1, 1}, mylib::Color{200, 100, 0});
    mylib::Color res = img.getPixel(mylib::Point{1, 1});
    EXPECT_EQ(res.b, 200);
    EXPECT_EQ(res.g, 100);
    EXPECT_EQ(res.r, 0);
}

TEST(ImageTest, ConstructorAndSize) {
    mylib::Image img(10, 10);
    EXPECT_EQ(img.width(), 10);
    EXPECT_EQ(img.height(), 10);
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            EXPECT_EQ(img.getPixel(mylib::Point{i, j}), (mylib::Color{255, 255, 255}));
        }
    }
}

TEST(ImageTest, inBounds) {
    mylib::Image img(10, 10);
    ASSERT_TRUE(img.inBounds(mylib::Point{1, 1}));
    ASSERT_TRUE(img.inBounds(mylib::Point{0, 0}));
    ASSERT_TRUE(img.inBounds(mylib::Point{9, 9}));
    ASSERT_TRUE(img.inBounds(mylib::Point{9, 0}));

    ASSERT_FALSE(img.inBounds(mylib::Point{10, 0}));
    ASSERT_FALSE(img.inBounds(mylib::Point{-1, 1}));
    ASSERT_FALSE(img.inBounds(mylib::Point{1, -1}));
    ASSERT_FALSE(img.inBounds(mylib::Point{-1, 100}));
}

TEST(ImageTest, ColorFormatTest) {
    mylib::Image img(10, 10);
    img.setPixel(mylib::Point{1, 1}, mylib::Color{255, 0, 0});
    EXPECT_EQ(img.getPixel(mylib::Point{1, 1}), (mylib::Color{255, 0, 0}));
    img.setPixel(mylib::Point{2, 2}, mylib::Color{0, 255, 0});
    EXPECT_EQ(img.getPixel(mylib::Point{2, 2}), (mylib::Color{0, 255, 0}));
    img.setPixel(mylib::Point{3, 3}, mylib::Color{0, 0, 255});
    EXPECT_EQ(img.getPixel(mylib::Point{3, 3}), (mylib::Color{0, 0, 255}));
}