#include <iostream>

#include "moonrock/moonrock.h"


int main() {
    moonrock::ImageUint2D img{ 32, 32 };

    for (int i = 0; i < img.width(); ++i) {
        auto& pix = img.pixel(i, 0);
        pix.set_color_xyzw(123, 14, 0, 12);
    }

    std::cout << moonrock::export_image_to_disk("C:\\Users\\woos8\\Downloads\\output.png", img) << std::endl;
    return 0;
}
