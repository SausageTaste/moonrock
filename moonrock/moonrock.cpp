#include "moonrock.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


namespace moonrock {

    bool export_image_to_disk(const char* const output_path, const ImageUint2D& img) {
        assert(4 == sizeof(Pixel4Uint8));
        return 0 != stbi_write_png(output_path, img.width(), img.height(), 4, img.data(), img.height() * 4);
    }

}
