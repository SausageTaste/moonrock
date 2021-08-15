#include "moonrock.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


// https://stackoverflow.com/a/2049593/6819534
namespace {

    using fPoint = moonrock::vec2;

    float sign (fPoint p1, fPoint p2, fPoint p3) {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    }

    bool PointInTriangle (fPoint pt, fPoint v1, fPoint v2, fPoint v3) {
        float d1, d2, d3;
        bool has_neg, has_pos;

        d1 = sign(pt, v1, v2);
        d2 = sign(pt, v2, v3);
        d3 = sign(pt, v3, v1);

        has_neg = (d1 <= 0) || (d2 <= 0) || (d3 <= 0);
        has_pos = (d1 >= 0) || (d2 >= 0) || (d3 >= 0);

        return !(has_neg && has_pos);
    }

}


// Pixel4Uint8
namespace moonrock {

    Pixel4Uint8::operator Pixel4Float32() const {
        Pixel4Float32 output;
        output.set_color_xyzw(this->x, this->y, this->z, this->w);
        return output;
    }

    void Pixel4Uint8::set_color_x(const float value) {
        this->x = static_cast<uint8_t>(std::min<float>(value, 1) * 255);
    }

    void Pixel4Uint8::set_color_y(const float value) {
        this->y = static_cast<uint8_t>(std::min<float>(value, 1) * 255);
    }

    void Pixel4Uint8::set_color_z(const float value) {
        this->z = static_cast<uint8_t>(std::min<float>(value, 1) * 255);
    }

    void Pixel4Uint8::set_color_w(const float value) {
        this->w = static_cast<uint8_t>(std::min<float>(value, 1) * 255);
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
        this->x = 0;
        this->y = 0;
        this->z = 0;
        this->w = 1;
    }

    Pixel4Float32::operator Pixel4Uint8() const {
        Pixel4Uint8 output;
        output.set_color_xyzw(this->x, this->y, this->z, this->w);
        return output;
    }

    void Pixel4Float32::set_color_x(const float value) {
        this->x = value;
    }

    void Pixel4Float32::set_color_y(const float value) {
        this->y = value;
    }

    void Pixel4Float32::set_color_z(const float value) {
        this->z = value;
    }

    void Pixel4Float32::set_color_w(const float value) {
        this->w = value;
    }

    void Pixel4Float32::set_color_xyzw(const float x, const float y, const float z, const float w) {
        this->set_color_x(x);
        this->set_color_y(y);
        this->set_color_z(z);
        this->set_color_w(w);
    }

}


// Rasterizer
namespace moonrock {

    void Rasterizer::work(std::vector<uvec2>& output) const {
        output.clear();

        const std::array<vec2, 3> edges{
            this->m_vertices[1] - this->m_vertices[0],
            this->m_vertices[2] - this->m_vertices[1],
            this->m_vertices[0] - this->m_vertices[2],
        };

        const std::array<vec2, 3> edges_normalized{
            edges[0].normalize(),
            edges[1].normalize(),
            edges[2].normalize(),
        };

        const std::array<vec2, 3> edges_normalized_ccw{
            edges_normalized[0].rotate_ccw_90(),
            edges_normalized[1].rotate_ccw_90(),
            edges_normalized[2].rotate_ccw_90(),
        };

        std::vector<std::pair<vec2, vec2>> left_edges;
        std::vector<std::pair<vec2, vec2>> top_edges;

        for (size_t i = 0; i < 3; ++i) {
            const auto v0 = this->m_vertices[i];
            const auto v1 = this->m_vertices[(i + 1) % 3];
            const auto e_n_ccw = edges_normalized_ccw[i];

            if (e_n_ccw.dot(vec2{-1, 0}) > 0) {
                left_edges.push_back(std::make_pair(v0, v1));
            }

            if (e_n_ccw.dot(vec2{0, -1}) == 1) {
                top_edges.push_back(std::make_pair(v0, v1));
            }
        }

        const auto [min, max] = this->make_min_max();

        for (uint32_t x = 0; x < this->m_domain_width; ++x) {
            for (uint32_t y = 0; y < this->m_domain_height; ++y) {
                vec2 sample_point{ static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f };
                if (::PointInTriangle(sample_point, this->m_vertices[0], this->m_vertices[1], this->m_vertices[2])) {
                    output.push_back(tvec2<uint32_t>{x, y});
                }
                else {
                    for (const auto& [v0, v1] : left_edges) {
                        // Check if the point is on the line using linear function `y = ax + b`
                        if ((sample_point.y - v0.y)*(v0.x - v1.x) == (v0.y - v1.y)*(sample_point.x - v0.x)) {
                            output.push_back(tvec2<uint32_t>{x, y});
                        }
                    }

                    for (const auto& [v0, v1] : top_edges) {
                        if ((sample_point.y - v0.y)*(v0.x - v1.x) == (v0.y - v1.y)*(sample_point.x - v0.x)) {
                            output.push_back(tvec2<uint32_t>{x, y});
                        }
                    }
                }
            }
        }
    }

    std::vector<uvec2> Rasterizer::work() const {
        std::vector<tvec2<uint32_t>> output;
        this->work(output);
        return output;
    }

    // Private

    std::pair<uvec2, uvec2> Rasterizer::make_min_max() const {
        vec2 min = this->m_vertices[0];
        vec2 max = this->m_vertices[0];

        for (size_t i = 1; i < this->m_vertices.size(); ++i) {
            const auto& v = this->m_vertices[i];

            min.x = std::min<float>(min.x, v.x);
            min.y = std::min<float>(min.y, v.y);

            max.x = std::max<float>(max.x, v.x);
            max.y = std::max<float>(max.y, v.y);
        }

        min.x = std::max<float>(min.x, 0);
        min.y = std::max<float>(min.y, 0);

        max.x = std::min<float>(max.x, this->m_domain_width);
        max.y = std::min<float>(max.y, this->m_domain_height);

        return std::make_pair(static_cast<uvec2>(min), static_cast<uvec2>(max));
    }

}


namespace moonrock {

    bool export_image_to_disk(const char* const output_path, const ImageUint2D& img) {
        assert(4 == sizeof(Pixel4Uint8));
        return 0 != stbi_write_png(output_path, img.width(), img.height(), 4, img.data(), img.height() * 4);
    }

}
