#pragma once

#include <array>
#include <string>
#include <optional>

#include "image.h"


namespace moonrock {

    class VertexStatic {

    public:
        glm::vec3 m_position;
        glm::vec3 m_normal;
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

    public:
        std::string m_albedo_map;
        float m_roughness;
        float m_metallic;
        bool m_alpha_blending;

        ImageUint2D* m_albedo_map_tex = nullptr;

    };


    template <typename _VertexTyp>
    class RenderUnit {

    public:
        VertexBuffer<_VertexTyp> m_mesh;
        Material m_material;
        glm::vec3 m_weight_center;

    };


    class ModelStatic {

    public:
        std::vector<RenderUnit<VertexStatic>> m_units;

    };


    void gen_mesh_quad(std::vector<VertexStatic>& output, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

    bool build_model_from_dmd(const uint8_t* const data, const size_t data_size, ModelStatic& output);

}
