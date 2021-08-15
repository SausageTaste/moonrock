#include <iostream>

#include "moonrock/moonrock.h"


int main() {
    moonrock::ImageUint2D img{ 512, 512 };
    moonrock::VertexBuffer vbuf;
    moonrock::Shader shader;

    vbuf.m_vertices.emplace_back();
    vbuf.m_vertices.back().m_position = glm::vec3{200, 30, 0};
    vbuf.m_vertices.back().m_color = glm::vec4{1, 0, 0, 1};

    vbuf.m_vertices.emplace_back();
    vbuf.m_vertices.back().m_position = glm::vec3{120, 600, 0};
    vbuf.m_vertices.back().m_color = glm::vec4{0, 1, 0, 1};

    vbuf.m_vertices.emplace_back();
    vbuf.m_vertices.back().m_position = glm::vec3{600, 80, 0};
    vbuf.m_vertices.back().m_color = glm::vec4{0, 0, 1, 1};

    shader.draw(vbuf, img);

    std::cout << moonrock::export_image_to_disk("C:\\Users\\woos8\\Downloads\\output.png", img) << std::endl;
    system("C:\\Users\\woos8\\Downloads\\output.png");
    return 0;
}
