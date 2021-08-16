#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <iostream>

#include <glm/glm.hpp>


namespace moonrock {

    class Vertex {

    public:
        glm::vec3 m_position;

    };


    class VertexBuffer {

    public:
        std::vector<Vertex> m_vertices;

    public:
        size_t size() const {
            return this->m_vertices.size();
        }

    };


    void gen_mesh_quad(std::vector<Vertex>& output, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

}
