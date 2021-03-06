#pragma once

#include <vector>
#include <cstdint>

#include "math_tool.h"


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

        Pixel4Uint8(const float x, const float y, const float z, const float w) {
            this->set_color_xyzw(x, y, z, w);
        }

        operator Pixel4Float32() const;

        float color_x() const;

        float color_y() const;

        float color_z() const;

        float color_w() const;

        glm::vec4 color_xyzw() const;

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

        glm::vec4 color_xyzw() const {
            return glm::vec4{
                this->color_x(),
                this->color_y(),
                this->color_z(),
                this->color_w()
            };
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
        Image2D()
            : m_width(0)
            , m_height(0)
        {

        }

        Image2D(const uint32_t width, const uint32_t height)
            : m_data(width * height)
            , m_width(width)
            , m_height(height)
        {

        }

        Image2D(Image2D&&) = default;

        Image2D& operator=(Image2D&&) = default;

        auto width() const {
            return this->m_width;
        }

        auto height() const {
            return this->m_height;
        }

        auto dimensions() const {
            return glm::vec2{ this->width(), this->height() };
        }

        auto data() {
            return this->m_data.data();
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

        glm::vec4 sample_nearest(const float x, const float y) const {
            return this->m_data[this->calc_index(x * (this->width() - 1), y * (this->height() - 1))].color_xyzw();
        }

        glm::vec4 sample_nearest(const glm::vec2& coords) const {
            return this->sample_nearest(coords.x, coords.y);
        }

        glm::vec4 sample_bilinear(const float x, const float y) const {
            const auto x_width = x * (this->width() - 1);
            const auto y_height = y * (this->height() - 1);

            float _;
            const auto x_frac = std::modf(x_width, &_);
            const auto y_frac = std::modf(y_height, &_);

            const auto x_floor = static_cast<uint32_t>(std::floor(x_width));
            const auto y_floor = static_cast<uint32_t>(std::floor(y_height));
            const auto x_ceiling = std::min(x_floor + 1, this->width() - 1);
            const auto y_ceiling = std::min(y_floor + 1, this->height() - 1);

            const auto lit00 = this->pixel(x_floor,   y_floor  ).color_xyzw();
            const auto lit01 = this->pixel(x_floor,   y_ceiling).color_xyzw();
            const auto lit10 = this->pixel(x_ceiling, y_floor  ).color_xyzw();
            const auto lit11 = this->pixel(x_ceiling, y_ceiling).color_xyzw();

            const auto lit_y0    = glm::mix( lit00,  lit10, x_frac);
            const auto lit_y1    = glm::mix( lit01,  lit11, x_frac);
            const auto lit_total = glm::mix(lit_y0, lit_y1, y_frac);

            return lit_total;
        }

        glm::vec4 sample_bilinear(const glm::vec2& coords) const {
            return this->sample_bilinear(coords.x, coords.y);
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
            return (x % this->width()) + (y % this->height()) * this->width();
        }

    };


    using ImageFloat2D = Image2D<Pixel4Float32>;
    using ImageUint2D = Image2D<Pixel4Uint8>;

}
