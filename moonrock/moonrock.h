#include <array>
#include <vector>
#include <cstdint>
#include <iostream>

#include "math_tool.h"


namespace moonrock {

    class Pixel4Float32;


    class Pixel4Uint8 : protected tvec4<uint8_t> {

    public:
        Pixel4Uint8() {
            this->x = 0;
            this->y = 0;
            this->z = 0;
            this->w = 255;
        }

        operator Pixel4Float32() const;

        void set_color_x(const float value);

        void set_color_y(const float value);

        void set_color_z(const float value);

        void set_color_w(const float value);

        void set_color_xyzw(const float x, const float y, const float z, const float w);

    };


    class Pixel4Float32 : protected tvec4<float> {

    public:
        using tvec4<float>::tvec4;
        using tvec4<float>::operator=;

        Pixel4Float32();

        operator Pixel4Uint8() const;

        void set_color_x(const float value);

        void set_color_y(const float value);

        void set_color_z(const float value);

        void set_color_w(const float value);

        void set_color_xyzw(const float x, const float y, const float z, const float w);

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

        auto& pixel(const uvec2 v) {
            return this->pixel(v.x, v.y);
        }

        auto& pixel(const uint32_t x, const uint32_t y) const {
            return this->m_data[this->calc_index(x, y)];
        }

        auto& pixel(const uvec2 v) const {
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
        std::array<vec2, 3> m_vertices;
        uint32_t m_domain_width = 0, m_domain_height = 0;

    public:
        void work(std::vector<uvec2>& output) const;

        std::vector<uvec2> work() const;

    private:
        std::pair<uvec2, uvec2> make_min_max() const;

    };


    class Vertex {

    public:
        vec3 m_position;
        Pixel4Float32 m_color;

    };


    class VertexBuffer {

    public:
        std::vector<Vertex> m_vertices;

    public:
        size_t size() const {
            return this->m_vertices.size();
        }

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
                this->m_rasterizer.m_vertices[0] = vert_buf.m_vertices[3 * i + 0].m_position.xy();
                this->m_rasterizer.m_vertices[1] = vert_buf.m_vertices[3 * i + 1].m_position.xy();
                this->m_rasterizer.m_vertices[2] = vert_buf.m_vertices[3 * i + 2].m_position.xy();

                for (auto v : this->m_rasterizer.work()) {
                    output_img.pixel(v) = static_cast<_PixelTyp>(vert_buf.m_vertices[3 * i + 0].m_color);
                }
            }
        }

    };

}
