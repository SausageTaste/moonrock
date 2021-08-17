#include <iostream>

#define STBI_NO_PSD
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "moonrock/moonrock.h"


namespace {

    moonrock::ImageUint2D load_image_from_disk(const char* const path) {
        static_assert(std::is_same<uint8_t, stbi_uc>::value);

        int width, height, channels;
        const auto pixels = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
        if (nullptr == pixels)
            throw std::exception{};

        const auto image_data_size = static_cast<size_t>(width * height * 4);

        moonrock::ImageUint2D output(width, height);
        memcpy(output.vector().data(), pixels, image_data_size);

        stbi_image_free(pixels);
        return output;
    }

}


int main() {
    moonrock::ImageUint2D img{ 512, 512 };
    moonrock::Image2D<moonrock::Pixel1Float32> depth_map{ 512, 512 };
    moonrock::VertexBuffer vbuf;
    moonrock::Shader shader;

    depth_map.fill(moonrock::Pixel1Float32{1});

    moonrock::gen_mesh_quad(vbuf.m_vertices, glm::vec3{-1, -1, 0}, glm::vec3{-1, 1, 0}, glm::vec3{1, 1, 0}, glm::vec3{1, -1, 0});
    moonrock::gen_mesh_quad(vbuf.m_vertices, glm::vec3{0, -1 + 0.3, -1}, glm::vec3{0, 1 + 0.3, -1}, glm::vec3{0, 1 + 0.3, 1}, glm::vec3{0, -1 + 0.3, 1});

    shader.draw(vbuf, img, depth_map);

    const auto texture = ::load_image_from_disk("C:\\Users\\woos8\\Downloads\\[mitsuba choco] __suruga_runa_high_school_fleet_drawn_by_mitsuba_choco__599aa97d9061bfcc8897a6a395795084.png");

    std::cout << moonrock::export_image_to_disk("C:\\Users\\woos8\\Downloads\\output_color.png", img) << std::endl;
    std::cout << moonrock::export_image_to_disk("C:\\Users\\woos8\\Downloads\\output_depth.png", depth_map.convert<moonrock::Pixel1Uint8>()) << std::endl;
    system("C:\\Users\\woos8\\Downloads\\output_color.png");
    return 0;
}
