#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"


namespace moonrock {

    class Pixel4Float32;


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

        void clear(const float x, const float y, const float z, const float w) {
            for (auto& pixel : this->m_data) {
                pixel.set_color_xyzw(x, y, z, w);
            }
        }

    private:
        size_t calc_index(const uint32_t x, const uint32_t y) const {
            return x + y * this->width();
        }

    };

    using ImageFloat2D = Image2D<Pixel4Float32>;
    using ImageUint2D = Image2D<Pixel4Uint8>;


    bool export_image_to_disk(const char* const output_path, const ImageUint2D& img);


    class Rasterizer {

    public:
        std::array<glm::vec2, 3> m_vertices;
        uint32_t m_domain_width = 0, m_domain_height = 0;

    public:
        void work(std::vector<glm::uvec2>& output) const;

        std::vector<glm::uvec2> work() const;

    private:
        std::pair<glm::uvec2, glm::uvec2> make_min_max() const;

    };


    class Shader {

    private:
        Rasterizer m_rasterizer;

    public:
        template <typename _PixelTyp>
        void draw(const VertexBuffer& vert_buf, Image2D<_PixelTyp>& output_img) {
            this->m_rasterizer.m_domain_width = output_img.width();
            this->m_rasterizer.m_domain_height = output_img.height();

            for (size_t i = 0; i < vert_buf.size() / 3; ++i) {
                this->m_rasterizer.m_vertices[0] = this->transform_vertex(vert_buf.m_vertices[3 * i + 0].m_position, output_img.width(), output_img.height());
                this->m_rasterizer.m_vertices[1] = this->transform_vertex(vert_buf.m_vertices[3 * i + 1].m_position, output_img.width(), output_img.height());
                this->m_rasterizer.m_vertices[2] = this->transform_vertex(vert_buf.m_vertices[3 * i + 2].m_position, output_img.width(), output_img.height());

                for (auto v : this->m_rasterizer.work()) {
                    output_img.pixel(v).set_color_xyzw(1, 1, 0, 1);
                }
            }
        }

    private:
        static glm::vec2 transform_vertex(const glm::vec3& v, const float width, const float height) {
            const auto model_mat = glm::rotate(glm::mat4{1}, glm::radians<float>(30), glm::vec3{0, 1, 0});
            const auto view_mat = glm::translate(glm::mat4{1}, glm::vec3{0, 0, -3});
            const auto proj_mat = glm::perspective<float>(glm::radians<float>(90), 1, 0.1, 100);
            const auto result = proj_mat * view_mat * model_mat * glm::vec4{v, 1};
            const auto in_device_space = glm::vec2{ result.x / result.w, result.y / result.w };
            const auto output = glm::vec2{ (in_device_space.x * 0.5 + 0.5) * width, (in_device_space.y * 0.5 + 0.5) * height };
            return output;
        }

    };

}
