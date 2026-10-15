#include "Color.h"

class Image
{
private:
    Color *pixelArray;
public:
    explicit Image(std::string filename);
    void setPixel(size_t horizontal, size_t vertical, Color color);
    void saveImage(std::string filename);
    ~Image();
};
