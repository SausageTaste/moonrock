#include <iostream>

#include "moonrock/moonrock.h"


int main() {
    moonrock::ImageUint2D img{ 512, 512 };
    moonrock::VertexBuffer vbuf;
    moonrock::Shader shader;

    moonrock::gen_mesh_quad(vbuf.m_vertices, glm::vec3{-1, -1, 0}, glm::vec3{-1, 1, 0}, glm::vec3{1, 1, 0}, glm::vec3{1, -1, 0});

    shader.draw(vbuf, img);

    std::cout << moonrock::export_image_to_disk("C:\\Users\\woos8\\Downloads\\output.png", img) << std::endl;
    system("C:\\Users\\woos8\\Downloads\\output.png");
    return 0;
}
