#include "image.h"

#include <algorithm>


// Pixel1Uint8
namespace moonrock {

    Pixel1Uint8::Pixel1Uint8(const float color) {
        this->set_color(color);
    }

    void Pixel1Uint8::operator=(const float value) {
        this->set_color(value);
    }

    Pixel1Uint8::operator Pixel1Float32() const {
        return Pixel1Float32{this->color()};
    }

    float Pixel1Uint8::color() const {
        return static_cast<float>(this->m_color) / 255.f;
    }

    void Pixel1Uint8::set_color(const float value) {
        this->m_color = static_cast<uint8_t>(std::min<float>(value, 1) * 255);
    }

}


// Pixel4Uint8
namespace moonrock {

    Pixel4Uint8::Pixel4Uint8() {
        this->m_color = glm::uvec4{0, 0, 0, 255};
    }

    Pixel4Uint8::operator Pixel4Float32() const {
        Pixel4Float32 output;
        output.set_color_xyzw(this->color_x(), this->color_y(), this->color_z(), this->color_w());
        return output;
    }

    float Pixel4Uint8::color_x() const {
        return static_cast<float>(this->m_color.x) / 255.f;
    }

    float Pixel4Uint8::color_y() const {
        return static_cast<float>(this->m_color.y) / 255.f;
    }

    float Pixel4Uint8::color_z() const {
        return static_cast<float>(this->m_color.z) / 255.f;
    }

    float Pixel4Uint8::color_w() const {
        return static_cast<float>(this->m_color.w) / 255.f;
    }

    glm::vec4 Pixel4Uint8::color_xyzw() const {
        return glm::vec4{
            this->color_x(),
            this->color_y(),
            this->color_z(),
            this->color_w()
        };
    }

    void Pixel4Uint8::set_color_x(const float value) {
        this->m_color.x = static_cast<uint8_t>(std::min<float>(value, 1) * 255);
    }

    void Pixel4Uint8::set_color_y(const float value) {
        this->m_color.y = static_cast<uint8_t>(std::min<float>(value, 1) * 255);
    }

    void Pixel4Uint8::set_color_z(const float value) {
        this->m_color.z = static_cast<uint8_t>(std::min<float>(value, 1) * 255);
    }

    void Pixel4Uint8::set_color_w(const float value) {
        this->m_color.w = static_cast<uint8_t>(std::min<float>(value, 1) * 255);
    }

    void Pixel4Uint8::set_color_xyzw(const float x, const float y, const float z, const float w) {
        this->set_color_x(x);
        this->set_color_y(y);
        this->set_color_z(z);
        this->set_color_w(w);
    }

}


// Pixel4Float32
namespace moonrock {

    Pixel4Float32::Pixel4Float32() {
        this->m_color = glm::vec4{0, 0, 0, 1};
    }

    Pixel4Float32::operator Pixel4Uint8() const {
        Pixel4Uint8 output;
        output.set_color_xyzw(this->color_x(), this->color_y(), this->color_z(), this->color_w());
        return output;
    }

    void Pixel4Float32::set_color_x(const float value) {
        this->m_color.x = value;
    }

    void Pixel4Float32::set_color_y(const float value) {
        this->m_color.y = value;
    }

    void Pixel4Float32::set_color_z(const float value) {
        this->m_color.z = value;
    }

    void Pixel4Float32::set_color_w(const float value) {
        this->m_color.w = value;
    }

    void Pixel4Float32::set_color_xyzw(const float x, const float y, const float z, const float w) {
        this->set_color_x(x);
        this->set_color_y(y);
        this->set_color_z(z);
        this->set_color_w(w);
    }

}
