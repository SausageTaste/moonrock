#include "render_things.h"

#include <iostream>

#include <direct.h>


int main() {
    RenderThings render_things;
    render_things.render();
    std::cout << "Done rendering" << std::endl;

    const auto output_path = moonrock::find_parent_folder_containing_folder_named("resource") + "/rendered";
    mkdir(output_path.c_str());

    const auto output_color_path = output_path + "/output_color.png";
    moonrock::export_image_to_disk(output_color_path.c_str(), render_things.m_fbuf.m_color_buffer);
    std::cout << "Done exporting" << std::endl;

    return 0;
}
