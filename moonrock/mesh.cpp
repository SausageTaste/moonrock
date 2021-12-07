#include "mesh.h"

#include <daltools/model_parser.h>


namespace {

    template <typename _VertType>
    glm::vec3 calc_weight_center(const std::vector<_VertType>& vertices) {
        double x_sum = 0.0;
        double y_sum = 0.0;
        double z_sum = 0.0;

        for (const auto& v : vertices) {
            x_sum += v.m_position.x;
            y_sum += v.m_position.y;
            z_sum += v.m_position.z;
        }

        const double vert_count = static_cast<double>(vertices.size());

        return glm::vec3{
            x_sum / vert_count,
            y_sum / vert_count,
            z_sum / vert_count,
        };
    }

    void copy_material(moonrock::Material& dst, const dal::parser::Material& src) {
        dst.m_roughness = src.m_roughness;
        dst.m_metallic = src.m_metallic;
        dst.m_albedo_map = src.m_albedo_map;
        dst.m_alpha_blending = src.alpha_blend;
    }

}


namespace moonrock {

    void gen_mesh_quad(std::vector<VertexStatic>& output, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
        output.push_back(VertexStatic{ p0, glm::vec3{0}, glm::vec2{0, 0} });
        output.push_back(VertexStatic{ p1, glm::vec3{0}, glm::vec2{0, 1} });
        output.push_back(VertexStatic{ p2, glm::vec3{0}, glm::vec2{1, 1} });

        output.push_back(VertexStatic{ p0, glm::vec3{0}, glm::vec2{0, 0} });
        output.push_back(VertexStatic{ p2, glm::vec3{0}, glm::vec2{1, 1} });
        output.push_back(VertexStatic{ p3, glm::vec3{0}, glm::vec2{1, 0} });
    }

    bool build_model_from_dmd(const uint8_t* const data, const size_t data_size, ModelStatic& output) {
        const auto unzipped = dal::parser::unzip_dmd(data, data_size);
        if (!unzipped)
            return false;

        const auto model_data = dal::parser::parse_dmd(unzipped->data(), unzipped->size());
        if (!model_data)
            return false;

        for (const auto& src_unit : model_data->m_units_indexed) {
            auto& dst_unit = output.m_units.emplace_back();
            dst_unit.m_mesh.m_vertices.reserve(src_unit.m_mesh.m_vertices.size());

            for (const auto index : src_unit.m_mesh.m_indices) {
                auto& dst_vert = dst_unit.m_mesh.m_vertices.emplace_back();
                auto& src_vert = src_unit.m_mesh.m_vertices[index];

                dst_vert.m_position = src_vert.m_position;
                dst_vert.m_normal = src_vert.m_normal;
                dst_vert.m_uv_coord = src_vert.m_uv_coords;
            }

            ::copy_material(dst_unit.m_material, src_unit.m_material);
            dst_unit.m_weight_center = ::calc_weight_center(dst_unit.m_mesh.m_vertices);
        }

        for (const auto& src_unit : model_data->m_units_indexed_joint) {
            auto& dst_unit = output.m_units.emplace_back();
            dst_unit.m_mesh.m_vertices.reserve(src_unit.m_mesh.m_vertices.size());

            for (const auto index : src_unit.m_mesh.m_indices) {
                auto& dst_vert = dst_unit.m_mesh.m_vertices.emplace_back();
                auto& src_vert = src_unit.m_mesh.m_vertices[index];

                dst_vert.m_position = src_vert.m_position;
                dst_vert.m_normal = src_vert.m_normal;
                dst_vert.m_uv_coord = src_vert.m_uv_coords;
            }

            ::copy_material(dst_unit.m_material, src_unit.m_material);
            dst_unit.m_weight_center = ::calc_weight_center(dst_unit.m_mesh.m_vertices);
        }

        return true;
    }

}
