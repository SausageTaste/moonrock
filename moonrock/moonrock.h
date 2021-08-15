#include <array>
#include <vector>
#include <cstdint>
#include <iostream>


namespace moonrock {

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
        uint64_t m_width, m_height;

    public:
        Image2D(const uint64_t width, const uint64_t height)
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

        auto& pixel(const uint64_t x, const uint64_t y) {
            return this->m_data[this->calc_index(x, y)];
        }

        auto& pixel(const uint64_t x, const uint64_t y) const {
            return this->m_data[this->calc_index(x, y)];
        }

    private:
        size_t calc_index(const uint64_t x, const uint64_t y) const {
            return x + y * this->width();
        }

    };

    using ImageFloat2D = Image2D<Pixel4Float32>;
    using ImageUint2D = Image2D<Pixel4Uint8>;


    bool export_image_to_disk(const char* const output_path, const ImageUint2D& img);

}
