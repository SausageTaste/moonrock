#include <iostream>
#include <fstream>

#include "moonrock/moonrock.h"


namespace {

    moonrock::ImageUint2D load_image_from_disk(const char* const path) {
        std::fstream file;
        file.open(path, std::ios::in | std::ios::ate | std::ios::binary);
        if (!file.is_open())
            throw std::exception{"failed to open file"};

        const auto content_size = file.tellg();
        file.seekg(0);

        std::vector<uint8_t> buffer(content_size);
        file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

        return moonrock::parse_image_from_memory(buffer.data(), buffer.size());
    }

}


int main() {
    moonrock::ImageUint2D img{ 1024, 1024 };
    moonrock::Image2D<moonrock::Pixel1Float32> depth_map{ 1024, 1024 };
    moonrock::VertexBuffer vbuf;
    moonrock::Shader shader;

    const auto texture = ::load_image_from_disk("C:\\Users\\woos8\\Downloads\\albedo_map.jpg");

    depth_map.fill(moonrock::Pixel1Float32{1});

    moonrock::gen_mesh_quad(vbuf.m_vertices, glm::vec3{-1, -1, 0}, glm::vec3{-1, 1, 0}, glm::vec3{1, 1, 0}, glm::vec3{1, -1, 0});
    moonrock::gen_mesh_quad(vbuf.m_vertices, glm::vec3{0, -1 + 0.3, -1}, glm::vec3{0, 1 + 0.3, -1}, glm::vec3{0, 1 + 0.3, 1}, glm::vec3{0, -1 + 0.3, 1});

    shader.draw(vbuf, texture, img, depth_map);

    std::cout << moonrock::export_image_to_disk("C:\\Users\\woos8\\Downloads\\output_color.png", img) << std::endl;
    std::cout << moonrock::export_image_to_disk("C:\\Users\\woos8\\Downloads\\output_depth.png", depth_map.convert<moonrock::Pixel1Uint8>()) << std::endl;
    system("C:\\Users\\woos8\\Downloads\\output_color.png");
    return 0;
}
