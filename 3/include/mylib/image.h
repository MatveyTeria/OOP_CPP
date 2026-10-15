#pragma once

#include "mylib/color.h"
#include "mylib/point.h"
#include <string>
#include <vector>
#include <cstdint>


namespace mylib
{
    
class Image
{
public:
    explicit Image(int width, int height, Color fill = {255, 255, 255});
    static Image loadBmp(const std::string& path);

    int width()  const noexcept { return _width; }
    int height() const noexcept { return _height; }

    bool inBounds(Point p) const noexcept {
        if (p.x < 0 || p.y < 0)
            return false;
        return (p.x < width() && p.y < height());
    }

    void setPixel(Point point, Color color) noexcept;
    Color getPixel(Point point) const noexcept;

    void saveBmp(const std::string& path) const;
private:
    int _width;
    int _height;
    std::vector<std::uint8_t> pixelArray; // store by rows BGR
    
    static constexpr int _channels = 3;

    std::size_t index(Point p) const {return _channels * (width() * p.y + p.x);}
};

} // namespace mylib
