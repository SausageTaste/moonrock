#include <iostream>

#include "moonrock/moonrock.h"


int main() {
    moonrock::ImageFloat2D img{ 32, 32 };
    std::cout << img.width() << " x " << img.height() << std::endl;
    std::cout << "at " << 16 << "x" << 16 << ", it's " << img.pixel(16, 16) << std::endl;
    return 0;
}
