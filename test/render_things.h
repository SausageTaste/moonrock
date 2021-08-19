#pragma once

#include <string>
#include <fstream>
#include <exception>

#include <moonrock/moonrock.h>
#include <moonrock/utils.h>


template <typename _Container>
_Container load_from_disk(const char* const path) {
    std::fstream file;
    file.open(path, std::ios::in | std::ios::ate | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error{"failed to open file"};

    const auto content_size = file.tellg();
    file.seekg(0);

    _Container buffer;
    buffer.resize(content_size);
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

    return buffer;
}

template <typename _Container>
_Container load_from_disk(const std::string path) {
    return load_from_disk<_Container>(path.c_str());
}

moonrock::ImageUint2D load_image_from_disk(const std::string& path);

std::string load_str_from_disk(const std::string& path);


class RenderThings {

public:
    moonrock::Framebuffer m_fbuf;
    moonrock::Shader m_shader;

    moonrock::ModelStatic m_model;
    moonrock::ImageUint2D m_texture;

public:
    RenderThings();

    void render();

};
