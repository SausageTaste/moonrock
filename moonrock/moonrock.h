#include <array>
#include <vector>
#include <cstdint>
#include <iostream>


namespace moonrock {

    class Pixel4Uint8 {

    public:
        std::array<uint8_t, 4> m_colors{ 0 }; 

    private:
        friend std::ostream& operator<<(std::ostream& os, const moonrock::Pixel4Uint8& obj) {
            os << "{" << obj.m_colors[0] << ", " << obj.m_colors[1] << ", " << obj.m_colors[2] << ", " << obj.m_colors[3] << "}";
            return os;
        }

    };


    class Pixel4Float32 {

    public:
        std::array<float, 4> m_colors{ 0 };

    private:
        friend std::ostream& operator<<(std::ostream& os, const moonrock::Pixel4Float32& obj) {
            os << "{" << obj.m_colors[0] << ", " << obj.m_colors[1] << ", " << obj.m_colors[2] << ", " << obj.m_colors[3] << "}";
            return os;
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

}
