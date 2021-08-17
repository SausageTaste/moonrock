#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <iostream>

#include "mesh.h"


namespace moonrock {

    class Pixel1Float32;
    class Pixel4Float32;


    class Pixel1Uint8 {

    private:
        uint8_t m_color = 0;

    public:
        Pixel1Uint8() = default;

        explicit
        Pixel1Uint8(const float color);

        void operator=(const float value);

        operator Pixel1Float32() const;

        float color() const;

        void set_color(const float value);

    };


    class Pixel1Float32 {

    private:
        float m_color = 0;

    public:
        Pixel1Float32() = default;

        explicit
        Pixel1Float32(const float color) {
            this->set_color(color);
        }

        void operator=(const float value) {
            this->set_color(value);
        }

        operator Pixel1Uint8() const {
            return Pixel1Uint8{this->color()};
        }

        float color() const {
            return this->m_color;
        }

        void set_color(const float value) {
            this->m_color = value;
        }

    };


    class Pixel4Uint8 {

    private:
        glm::tvec4<uint8_t> m_color;

    public:
        Pixel4Uint8();

        operator Pixel4Float32() const;

        float color_x() const;

        float color_y() const;

        float color_z() const;

        float color_w() const;

        void set_color_x(const float value);

        void set_color_y(const float value);

        void set_color_z(const float value);

        void set_color_w(const float value);

        void set_color_xyzw(const float x, const float y, const float z, const float w);

        void set_color_xyzw(const glm::vec4& v) {
            this->set_color_xyzw(v.x, v.y, v.z, v.w);
        }

    };


    class Pixel4Float32 {

    private:
        glm::vec4 m_color;

    public:
        Pixel4Float32();

        operator Pixel4Uint8() const;

        void operator=(const glm::vec4 v) {
            this->m_color = v;
        }

        float color_x() const {
            return this->m_color.x;
        }

        float color_y() const {
            return this->m_color.y;
        }

        float color_z() const {
            return this->m_color.z;
        }

        float color_w() const {
            return this->m_color.w;
        }

        void set_color_x(const float value);

        void set_color_y(const float value);

        void set_color_z(const float value);

        void set_color_w(const float value);

        void set_color_xyzw(const float x, const float y, const float z, const float w);

        void set_color_xyzw(const glm::vec4& v) {
            this->set_color_xyzw(v.x, v.y, v.z, v.w);
        }

    };


    template <typename _PixelTyp>
    class Image2D {

    private:
        std::vector<_PixelTyp> m_data;
        uint32_t m_width, m_height;

    public:
        Image2D(const uint32_t width, const uint32_t height)
            : m_data(width * height)
            , m_width(width)
            , m_height(height)
        {

        }

        auto width() const {
            return this->m_width;
        }

        auto height() const {
            return this->m_height;
        }

        auto data() const {
            return this->m_data.data();
        }

        auto& vector() {
            return this->m_data;
        }

        auto& vector() const {
            return this->m_data;
        }

        auto& pixel(const uint32_t x, const uint32_t y) {
            return this->m_data[this->calc_index(x, y)];
        }

        auto& pixel(const glm::uvec2 v) {
            return this->pixel(v.x, v.y);
        }

        auto& pixel(const uint32_t x, const uint32_t y) const {
            return this->m_data[this->calc_index(x, y)];
        }

        auto& pixel(const glm::uvec2 v) const {
            return this->pixel(v.x, v.y);
        }

        void fill(const _PixelTyp& value) {
            for (auto& x : this->vector()) {
                x = value;
            }
        }

        template <typename _DstPixelTyp>
        auto convert() {
            Image2D<_DstPixelTyp> output{ this->width(), this->height() };
            for (size_t i = 0; i < output.vector().size(); ++i) {
                output.vector().at(i) = static_cast<_DstPixelTyp>(this->vector().at(i));
            }
            return output;
        }

    private:
        size_t calc_index(const uint32_t x, const uint32_t y) const {
            return x + y * this->width();
        }

    };

    using ImageFloat2D = Image2D<Pixel4Float32>;
    using ImageUint2D = Image2D<Pixel4Uint8>;


    bool export_image_to_disk(const char* const output_path, const ImageUint2D& img);

    bool export_image_to_disk(const char* const output_path, const Image2D<Pixel1Uint8>& img);


    class Rasterizer {

    private:
        struct RasResult {
            glm::vec3 m_barycentric_coords;
            glm::uvec2 m_coord;
        };

    public:
        std::array<glm::vec2, 3> m_vertices;
        uint32_t m_domain_width = 0, m_domain_height = 0;

    public:
        void work(std::vector<RasResult>& output) const;

        std::vector<RasResult> work() const;

    private:
        std::pair<glm::uvec2, glm::uvec2> make_min_max() const;

    };


    class Shader {

    private:
        Rasterizer m_rasterizer;

    public:
        template <typename _PixelTyp>
        void draw(const VertexBuffer& vert_buf, Image2D<_PixelTyp>& output_img, Image2D<Pixel1Float32>& depth_map) {
            std::array<glm::vec4, 4> colors{
                glm::vec4{1, 0, 0, 1},
                glm::vec4{1, 0, 0, 1},
                glm::vec4{0, 1, 0, 1},
                glm::vec4{0, 1, 0, 1},
            };

            this->m_rasterizer.m_domain_width = output_img.width();
            this->m_rasterizer.m_domain_height = output_img.height();

            for (size_t i = 0; i < vert_buf.size() / 3; ++i) {
                const auto v0 = this->transform_vertex(vert_buf.m_vertices[3 * i + 0].m_position);
                const auto v1 = this->transform_vertex(vert_buf.m_vertices[3 * i + 1].m_position);
                const auto v2 = this->transform_vertex(vert_buf.m_vertices[3 * i + 2].m_position);

                this->m_rasterizer.m_vertices[0] = glm::vec2{ (v0.x * 0.5 + 0.5) * output_img.width(), (v0.y * 0.5 + 0.5) * output_img.height() };
                this->m_rasterizer.m_vertices[1] = glm::vec2{ (v1.x * 0.5 + 0.5) * output_img.width(), (v1.y * 0.5 + 0.5) * output_img.height() };
                this->m_rasterizer.m_vertices[2] = glm::vec2{ (v2.x * 0.5 + 0.5) * output_img.width(), (v2.y * 0.5 + 0.5) * output_img.height() };

                for (auto v : this->m_rasterizer.work()) {
                    const auto current_depth = glm::dot(v.m_barycentric_coords, glm::vec3{v0.z, v1.z, v2.z});
                    auto& depth_pixel = depth_map.pixel(v.m_coord);

                    if (current_depth < depth_pixel.color()) {
                        output_img.pixel(v.m_coord).set_color_xyzw(colors[i % colors.size()]);
                        depth_pixel = v0.z * v.m_barycentric_coords[0] + v1.z * v.m_barycentric_coords[1] + v2.z * v.m_barycentric_coords[2];
                    }
                }
            }
        }

    private:
        static glm::vec3 transform_vertex(const glm::vec3& v);

    };

}
