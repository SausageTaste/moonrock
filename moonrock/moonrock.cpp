#include "moonrock.h"

#include <limits>
#include <chrono>

#define STBI_NO_PSD
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>


#include "utils.h"
#include "math_tool.h"


// https://stackoverflow.com/a/2049593/6819534
namespace {

    using fPoint = glm::vec2;

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


// Rasterizer
namespace moonrock {

    void Rasterizer::work(result_list_t& output) const {
        output.clear();

        const std::array<glm::vec2, 3> edges{
            this->m_vertices[1] - this->m_vertices[0],
            this->m_vertices[2] - this->m_vertices[1],
            this->m_vertices[0] - this->m_vertices[2],
        };

        const std::array<glm::vec2, 3> edges_normalized{
            glm::normalize(edges[0]),
            glm::normalize(edges[1]),
            glm::normalize(edges[2]),
        };

        const std::array<glm::vec2, 3> edges_normalized_ccw{
            rotate_vec2_ccw_90(edges_normalized[0]),
            rotate_vec2_ccw_90(edges_normalized[1]),
            rotate_vec2_ccw_90(edges_normalized[2]),

        };

        std::vector<std::pair<glm::vec2, glm::vec2>> left_edges;
        std::vector<std::pair<glm::vec2, glm::vec2>> top_edges;

        for (size_t i = 0; i < 3; ++i) {
            const auto v0 = this->m_vertices[i];
            const auto v1 = this->m_vertices[(i + 1) % 3];
            const auto e_n_ccw = edges_normalized_ccw[i];

            if (glm::dot(e_n_ccw, glm::vec2{-1, 0}) > 0.f) {
                left_edges.push_back(std::make_pair(v0, v1));
            }

            if (glm::dot(e_n_ccw, glm::vec2{0, -1}) == 1.f) {
                top_edges.push_back(std::make_pair(v0, v1));
            }
        }

        const auto [min, max] = this->make_min_max();
        const auto triangle_area_times_2 = ::sign(this->m_vertices[0], this->m_vertices[1], this->m_vertices[2]);

        for (uint32_t x = min.x; x < max.x; ++x) {
            for (uint32_t y = min.y; y < max.y; ++y) {
                const glm::vec2 sample_point{ static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f };

                if (::PointInTriangle(sample_point, this->m_vertices[0], this->m_vertices[1], this->m_vertices[2])) {
                    output.push_back(this->make_one_result(x, y, sample_point, triangle_area_times_2));
                }
                else {
                    for (const auto& [v0, v1] : left_edges) {
                        // Check if the point is on the line using linear function `y = ax + b`
                        if ((sample_point.y - v0.y)*(v0.x - v1.x) == (v0.y - v1.y)*(sample_point.x - v0.x)) {
                            output.push_back(this->make_one_result(x, y, sample_point, triangle_area_times_2));
                        }
                    }

                    for (const auto& [v0, v1] : top_edges) {
                        if ((sample_point.y - v0.y)*(v0.x - v1.x) == (v0.y - v1.y)*(sample_point.x - v0.x)) {
                            output.push_back(this->make_one_result(x, y, sample_point, triangle_area_times_2));
                        }
                    }
                }
            }
        }
    }

    Rasterizer::result_list_t Rasterizer::work() const {
        result_list_t output;
        this->work(output);
        return output;
    }

    // Private

    std::pair<glm::uvec2, glm::uvec2> Rasterizer::make_min_max() const {
        glm::vec2 min = this->m_vertices[0];
        glm::vec2 max = this->m_vertices[0];

        for (size_t i = 1; i < this->m_vertices.size(); ++i) {
            const auto& v = this->m_vertices[i];

            min.x = std::min<float>(min.x, v.x);
            min.y = std::min<float>(min.y, v.y);

            max.x = std::max<float>(max.x, v.x);
            max.y = std::max<float>(max.y, v.y);
        }

        min.x = std::max<float>(min.x, 0);
        min.y = std::max<float>(min.y, 0);

        max.x = std::min<float>(max.x, static_cast<float>(this->m_domain_width));
        max.y = std::min<float>(max.y, static_cast<float>(this->m_domain_height));

        return std::make_pair(static_cast<glm::uvec2>(min), static_cast<glm::uvec2>(max));
    }

    Rasterizer::RasResult Rasterizer::make_one_result(
        const uint32_t x,
        const uint32_t y,
        const glm::vec2 sample_point,
        const float triangle_area_times_2
    ) const {
        RasResult result;

        result.m_coord = glm::uvec2{x, y};
        result.m_barycentric_coords[0] = ::sign(this->m_vertices[1], this->m_vertices[2], sample_point) / triangle_area_times_2;
        result.m_barycentric_coords[1] = ::sign(this->m_vertices[2], this->m_vertices[0], sample_point) / triangle_area_times_2;
        result.m_barycentric_coords[2] = ::sign(this->m_vertices[0], this->m_vertices[1], sample_point) / triangle_area_times_2;

        return result;
    }

}


namespace moonrock {

    bool export_image_to_disk(const char* const output_path, const ImageUint2D& img) {
        static_assert(4 == sizeof(Pixel4Uint8));
        return 0 != stbi_write_png(output_path, img.width(), img.height(), 4, img.data(), img.width() * 4);
    }

    bool export_image_to_disk(const char* const output_path, const Image2D<Pixel1Uint8>& img) {
        return 0 != stbi_write_png(output_path, img.width(), img.height(), 1, img.data(), img.width());
    }

    ImageUint2D parse_image_from_memory(const uint8_t* const buf, const size_t buf_size) {
        static_assert(std::is_same<uint8_t, stbi_uc>::value);
        assert(buf_size <= std::numeric_limits<int>::max());

        int width, height, channels;
        const auto pixels = stbi_load_from_memory(buf, static_cast<int>(buf_size), &width, &height, &channels, STBI_rgb_alpha);
        if (nullptr == pixels)
            throw std::exception{};

        const auto image_data_size = static_cast<size_t>(width * height * 4);

        moonrock::ImageUint2D output(width, height);
        memcpy(output.vector().data(), pixels, image_data_size);

        stbi_image_free(pixels);
        return output;
    }

}


// Shader
namespace moonrock {

    void Shader::draw(const VertexBuffer& vert_buf, const ImageUint2D& albedo_map, ImageUint2D& output_img, Image2D<Pixel1Float32>& depth_map) {
        assert(output_img.dimensions() == depth_map.dimensions());

        const auto cur_sec = static_cast<float>(get_cur_sec());
        const auto half_width = output_img.width() * 0.5;
        const auto half_height = output_img.height() * 0.5;
        Rasterizer::result_list_t rasterized;

        this->m_rasterizer.m_domain_width = output_img.width();
        this->m_rasterizer.m_domain_height = output_img.height();

        for (size_t i = 0; i < vert_buf.size() / 3; ++i) {
            const auto p0 = vert_buf.m_vertices[3 * i + 0];
            const auto p1 = vert_buf.m_vertices[3 * i + 1];
            const auto p2 = vert_buf.m_vertices[3 * i + 2];

            const auto v0 = this->transform_vertex(p0.m_position, cur_sec);
            const auto v1 = this->transform_vertex(p1.m_position, cur_sec);
            const auto v2 = this->transform_vertex(p2.m_position, cur_sec);

            this->m_rasterizer.m_vertices[0] = glm::vec2{ (v0.x * half_width + half_width), (v0.y * half_height + half_height) };
            this->m_rasterizer.m_vertices[1] = glm::vec2{ (v1.x * half_width + half_width), (v1.y * half_height + half_height) };
            this->m_rasterizer.m_vertices[2] = glm::vec2{ (v2.x * half_width + half_width), (v2.y * half_height + half_height) };
            this->m_rasterizer.work(rasterized);

            for (auto v : rasterized) {
                const auto current_depth = 1.f / interpolate_barycentric(1.f / v0.z, 1.f / v1.z, 1.f / v2.z, v.m_barycentric_coords);
                auto& depth_pixel = depth_map.pixel(v.m_coord);

                if (current_depth < depth_pixel.color()) {
                    const auto current_uv_coord = interpolate_barycentric(p0.m_uv_coord, p1.m_uv_coord, p2.m_uv_coord, v.m_barycentric_coords);
                    const auto current_albedo = albedo_map.sample_bilinear(current_uv_coord);
                    output_img.pixel(v.m_coord).set_color_xyzw(current_albedo);
                    depth_pixel = current_depth;
                }
            }
        }
    }

    glm::vec3 Shader::transform_vertex(const glm::vec3& v, const float seed) {
        const auto model_mat = glm::rotate(glm::mat4{1}, glm::radians<float>(seed * 10), glm::vec3{0, 1, 0});
        const auto view_mat = glm::translate(glm::mat4{1}, glm::vec3{0, 0, -3});
        const auto proj_mat = glm::perspective<float>(glm::radians(90.f), 1.f, 0.1f, 100.f);
        const auto transformed = proj_mat * view_mat * model_mat * glm::vec4{v, 1};
        const auto perspective_devided = glm::vec3{ transformed } / transformed.w;
        return perspective_devided;
    }

}
