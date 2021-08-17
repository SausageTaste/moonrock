#include <iostream>

#include "moonrock/moonrock.h"


int main() {
    moonrock::ImageUint2D img{ 512, 512 };
    moonrock::Image2D<moonrock::Pixel1Float32> depth_map{ 512, 512 };
    moonrock::VertexBuffer vbuf;
    moonrock::Shader shader;

    depth_map.fill(moonrock::Pixel1Float32{1});

    moonrock::gen_mesh_quad(vbuf.m_vertices, glm::vec3{-1, -1, 0}, glm::vec3{-1, 1, 0}, glm::vec3{1, 1, 0}, glm::vec3{1, -1, 0});
    moonrock::gen_mesh_quad(vbuf.m_vertices, glm::vec3{0, -1 + 0.3, -1}, glm::vec3{0, 1 + 0.3, -1}, glm::vec3{0, 1 + 0.3, 1}, glm::vec3{0, -1 + 0.3, 1});

    shader.draw(vbuf, img, depth_map);

    std::cout << moonrock::export_image_to_disk("C:\\Users\\woos8\\Downloads\\output.png", img) << std::endl;
    system("C:\\Users\\woos8\\Downloads\\output.png");
    return 0;
}
