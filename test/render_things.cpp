#include "render_things.h"


moonrock::ImageUint2D load_image_from_disk(const std::string& path) {
    const auto buffer = load_from_disk<std::vector<uint8_t>>(path);
    return moonrock::parse_image_from_memory(buffer.data(), buffer.size());
}

std::string load_str_from_disk(const std::string& path) {
    return ::load_from_disk<std::string>(path);
}


RenderThings::RenderThings()
    : m_fbuf(512, 512)
{
    const auto resource_path = moonrock::find_parent_folder_containing_folder_named("resource") + "/resource";
    const auto model_buffer = load_from_disk<std::vector<uint8_t>>(resource_path + "/Character Running.dmd");
    this->m_model = moonrock::build_model_from_dmd(model_buffer.data(), model_buffer.size()).value();
    this->m_texture = load_image_from_disk("C:\\Users\\woos8\\Downloads\\albedo_map.jpg");
}

void RenderThings::render() {
    this->m_fbuf.m_color_buffer.fill(moonrock::Pixel4Uint8(0, 0, 0, 1));
    this->m_fbuf.m_depth_map.fill(moonrock::Pixel1Float32{1});
    this->m_shader.draw(this->m_model.m_units.front().m_mesh, this->m_texture, this->m_fbuf);
}
