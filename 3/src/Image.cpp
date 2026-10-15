#include <stdexcept>
#include <cstddef>

#include "mylib/image.h"
#include "stb_image.h"
#include "stb_image_write.h"

namespace mylib {

Image::Image(int width, int height, Color fill) 
    : _width(width), _height(height),
    pixelArray(static_cast<std::size_t>(width) * height * _channels) {
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            setPixel({i, j}, fill);
        }
    }
}

Image Image::loadBmp(const std::string& path) {
    int w, h, channels;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 3);
    if (!data)
        throw std::runtime_error("cannot load: " + path);
    Image img(w, h);
    img.pixelArray.assign(data, data + static_cast<std::size_t>(w) * h * 3);
    stbi_image_free(data);
    return img;
}

void Image::setPixel(Point point, Color color) noexcept  {
    if (!inBounds(point))
        return;
    std::size_t ind = index(point);
    pixelArray[ind] = color.r;
    pixelArray[ind + 1] = color.g;
    pixelArray[ind + 2] = color.b;
}

Color Image::getPixel(Point point) const noexcept {
    if (!inBounds(point))
        return Color{};
    Color res;
    std::size_t ind = index(point);
    res.r = pixelArray[ind];
    res.g = pixelArray[ind + 1];
    res.b = pixelArray[ind + 2];
    return res;
}

void Image::saveBmp(const std::string& path) const {
    int ok = stbi_write_bmp(path.c_str(), _width, _height, 3, pixelArray.data());
    if (!ok)
        throw std::runtime_error("cannot save: " + path);
}

} // namespace mylib