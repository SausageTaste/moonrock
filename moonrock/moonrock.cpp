#include "moonrock.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


namespace moonrock {

    Pixel4Uint8::operator Pixel4Float32() const {
        Pixel4Float32 output;
        output.set_color_xyzw(this->x, this->y, this->z, this->w);
        return output;
    }

    bool export_image_to_disk(const char* const output_path, const ImageUint2D& img) {
        assert(4 == sizeof(Pixel4Uint8));
        return 0 != stbi_write_png(output_path, img.width(), img.height(), 4, img.data(), img.height() * 4);
    }

}
