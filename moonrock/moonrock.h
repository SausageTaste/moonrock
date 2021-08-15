#include <array>
#include <vector>
#include <cstdint>
#include <iostream>


namespace moonrock {

    template <typename T>
    class tvec2 {

    public:
        T x = 0;
        T y = 0;

    public:
        tvec2<T> operator+(tvec2<T> other) const {
            return tvec2<T>{ this->x + other.x, this->y + other.y };
        }

        tvec2<T> operator-(tvec2<T> other) const {
            return tvec2<T>{ this->x - other.x, this->y - other.y };
        }

        tvec2<T> rotate_cw_90() const {
            return tvec2<T>{ y, -x };
        }

        tvec2<T> rotate_ccw_90() const {
            return tvec2<T>{ -y, x };
        }

        auto dot(tvec2<T> other) const {
            return this->x * other.x + this->y * other.y;
        }

    private:
        friend std::ostream& operator<<(std::ostream& os, const moonrock::tvec2<T>& obj) {
            os << "{" << obj.x << ", " << obj.y << "}";
            return os;
        }

    };

    using vec2 = tvec2<float>;


    template <typename T>
    class tvec4 {

    public:
        T x = 0;
        T y = 0;
        T z = 0;
        T w = 0;

    private:
        friend std::ostream& operator<<(std::ostream& os, const moonrock::tvec4<T>& obj) {
            os << "{" << obj.x << ", " << obj.y << ", " << obj.z << ", " << obj.w << "}";
            return os;
        }

    };

    using vec4 = tvec4<float>;


    class Pixel4Uint8 : protected tvec4<uint8_t> {

    public:
        Pixel4Uint8() {
            this->x = 0;
            this->y = 0;
            this->z = 0;
            this->w = 255;
        }

        void set_color_x(const float value) {
            this->x = static_cast<uint8_t>(std::min<float>(value, 1) * 255);
        }

        void set_color_y(const float value) {
            this->y = static_cast<uint8_t>(std::min<float>(value, 1) * 255);
        }

        void set_color_z(const float value) {
            this->z = static_cast<uint8_t>(std::min<float>(value, 1) * 255);
        }

        void set_color_w(const float value) {
            this->w = static_cast<uint8_t>(std::min<float>(value, 1) * 255);
        }

        void set_color_xyzw(const float x, const float y, const float z, const float w) {
            this->set_color_x(x);
            this->set_color_y(y);
            this->set_color_z(z);
            this->set_color_w(w);
        }

    };


    class Pixel4Float32 : protected tvec4<float> {

    public:
        Pixel4Float32() {
            this->x = 0;
            this->y = 0;
            this->z = 0;
            this->w = 1;
        }

        void set_color_x(const float value) {
            this->x = value;
        }

        void set_color_y(const float value) {
            this->y = value;
        }

        void set_color_z(const float value) {
            this->z = value;
        }

        void set_color_w(const float value) {
            this->w = value;
        }

        void set_color_xyzw(const float x, const float y, const float z, const float w) {
            this->set_color_x(x);
            this->set_color_y(y);
            this->set_color_z(z);
            this->set_color_w(w);
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

        auto& pixel(const uint32_t x, const uint32_t y) const {
            return this->m_data[this->calc_index(x, y)];
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
        uint32_t domain_width = 0, domain_height = 0;

    public:
        std::pair<vec2, vec2> make_min_max() const {
            vec2 min = this->m_vertices[0];
            vec2 max = this->m_vertices[0];

            for (size_t i = 1; i < this->m_vertices.size(); ++i) {
                const auto& v = this->m_vertices[i];

                if (v.x < min.x)
                    min.x = v.x;
                if (v.y < min.y)
                    min.y = v.y;

                if (v.x > max.x)
                    max.x = v.x;
                if (v.y > max.y)
                    max.y = v.y;
            }

            return std::make_pair(min, max);
        }

        bool is_pix_inside(const float x, const float y) const {
            return this->PointInTriangle(vec2{ x, y }, this->m_vertices[0], this->m_vertices[1], this->m_vertices[2]);
        }

        void work(std::vector<tvec2<uint32_t>>& output) const {
            output.clear();

            const auto [min, max] = this->make_min_max();

            for (uint32_t x = min.x; x <= max.x; ++x) {
                for (uint32_t y = min.y; y < max.y; ++y) {
                    if (this->is_pix_inside(x, y)) {
                        output.push_back(tvec2<uint32_t>{x, y});
                    }
                }
            }
        }

        std::vector<tvec2<uint32_t>> work() const {
            std::vector<tvec2<uint32_t>> output;
            this->work(output);
            return output;
        }

    private:
        // https://stackoverflow.com/a/2049593/6819534
        static float sign(vec2 p1, vec2 p2, vec2 p3) {
            return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
        }

        static bool PointInTriangle(vec2 pt, vec2 v1, vec2 v2, vec2 v3) {
            float d1, d2, d3;
            bool has_neg, has_pos;

            d1 = sign(pt, v1, v2);
            d2 = sign(pt, v2, v3);
            d3 = sign(pt, v3, v1);

            has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
            has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

            return !(has_neg && has_pos);
        }

    };

}
