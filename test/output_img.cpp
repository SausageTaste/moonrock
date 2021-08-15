#include <iostream>

#include "moonrock/moonrock.h"


int main() {
    moonrock::ImageUint2D img{ 512, 512 };
    img.clear(1, 1, 1, 1);

    moonrock::Rasterizer rasterizer;
    rasterizer.domain_width = img.width();
    rasterizer.domain_height = img.height();
    rasterizer.m_vertices[0] = moonrock::vec2{200, 30};
    rasterizer.m_vertices[1] = moonrock::vec2{120, 300};
    rasterizer.m_vertices[2] = moonrock::vec2{420, 80};

    for (auto v : rasterizer.work()) {
        img.pixel(v.x, v.y).set_color_y(0);
        img.pixel(v.x, v.y).set_color_z(0);
    }

    rasterizer.m_vertices[0] = moonrock::vec2{420, 80};
    rasterizer.m_vertices[1] = moonrock::vec2{120, 300};
    rasterizer.m_vertices[2] = moonrock::vec2{480, 500};

    for (auto v : rasterizer.work()) {
        img.pixel(v.x, v.y).set_color_x(0);
        img.pixel(v.x, v.y).set_color_y(0);
    }

    std::cout << moonrock::export_image_to_disk("C:\\Users\\woos8\\Downloads\\output.png", img) << std::endl;
    return 0;
}
