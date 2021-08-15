#include <iostream>

#include "moonrock/moonrock.h"


int main() {
    moonrock::ImageUint2D img{ 512, 512 };
    moonrock::VertexBuffer vbuf;
    moonrock::Shader shader;

    vbuf.m_vertices.emplace_back();
    vbuf.m_vertices.back().m_position = moonrock::vec3{200, 30, 0};
    vbuf.m_vertices.back().m_color = moonrock::vec4{1, 0, 0, 1};

    vbuf.m_vertices.emplace_back();
    vbuf.m_vertices.back().m_position = moonrock::vec3{120, 200, 0};
    vbuf.m_vertices.back().m_color = moonrock::vec4{0, 1, 0, 1};

    vbuf.m_vertices.emplace_back();
    vbuf.m_vertices.back().m_position = moonrock::vec3{420, 80, 0};
    vbuf.m_vertices.back().m_color = moonrock::vec4{0, 0, 1, 1};

    shader.draw(vbuf, img);

    std::cout << moonrock::export_image_to_disk("C:\\Users\\woos8\\Downloads\\output.png", img) << std::endl;
    return 0;
}
