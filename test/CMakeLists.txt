
add_executable(moonrock_output_img
    output_img.cpp
)

target_include_directories(moonrock_output_img
    PRIVATE
        ${fetch_moonrock_stb_SOURCE_DIR}
)

target_link_libraries(moonrock_output_img
    moonrock
)
