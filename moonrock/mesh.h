#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <iostream>

#include "image.h"


namespace moonrock {

    class VertexStatic {

    public:
        glm::vec3 m_position;
        glm::vec2 m_uv_coord;

    };


    template <typename _VertexTyp>
    class VertexBuffer {

    public:
        std::vector<_VertexTyp> m_vertices;

    public:
        size_t size() const {
            return this->m_vertices.size();
        }

    };


    class Material {
        ImageUint2D* m_albedo_map = nullptr;
    };


    template <typename _VertexTyp>
    class RenderUnit {

    public:
        VertexBuffer<_VertexTyp> m_mesh;
        Material m_material;

    };


    class ModelStatic {

    public:
        std::vector<RenderUnit<VertexStatic>> m_units;

    };


    void gen_mesh_quad(std::vector<VertexStatic>& output, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

    void build_model_from_dmd(const uint8_t* const buf, const size_t buf_size);

}
