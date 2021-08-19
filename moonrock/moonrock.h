#pragma once

#include <cmath>
#include <array>
#include <vector>
#include <cstdint>
#include <iostream>

#include "mesh.h"


namespace moonrock {

    bool export_image_to_disk(const char* const output_path, const ImageUint2D& img);

    bool export_image_to_disk(const char* const output_path, const Image2D<Pixel1Uint8>& img);

    ImageUint2D parse_image_from_memory(const uint8_t* const buf, const size_t buf_size);


    class Framebuffer {

    public:
        moonrock::ImageUint2D m_color_buffer;
        moonrock::Image2D<moonrock::Pixel1Float32> m_depth_map;

    public:
        Framebuffer(const uint32_t width, const uint32_t height)
            : m_color_buffer(width, height)
            , m_depth_map(width, height)
        {

        }

        auto width() const {
            return this->m_color_buffer.width();
        }

        auto height() const {
            return this->m_color_buffer.height();
        }

    };


    class Rasterizer {

    private:
        struct RasResult {
            glm::vec3 m_barycentric_coords;
            glm::uvec2 m_coord;
        };

    public:
        using result_list_t = std::vector<RasResult>;

    public:
        std::array<glm::vec2, 3> m_vertices;
        uint32_t m_domain_width = 0, m_domain_height = 0;

    public:
        void work(result_list_t& output) const;

        result_list_t work() const;

    private:
        std::pair<glm::uvec2, glm::uvec2> make_min_max() const;

        RasResult make_one_result(
            const uint32_t x,
            const uint32_t y,
            const glm::vec2 sample_point,
            const float triangle_area_times_2
        ) const;

    };


    class Shader {

    private:
        Rasterizer m_rasterizer;

    public:
        void draw(const VertexBuffer<VertexStatic>& vert_buf, const ImageUint2D& albedo_map, Framebuffer& output);

    private:
        static glm::vec4 transform_vertex(const glm::vec3& v, const float seed);

    };

}
