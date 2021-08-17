
add_executable(moonrock_output_img
    output_img.cpp
)

target_link_libraries(moonrock_output_img
    moonrock
)


add_executable(moonrock_output_window
    output_window.cpp
)

target_link_libraries(moonrock_output_window
    moonrock
    glad
    glfw
)
