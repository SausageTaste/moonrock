
add_executable(moonrock_output_window
    render_things.h             render_things.cpp
    output_window.cpp
)

target_link_libraries(moonrock_output_window
    moonrock
    glad
    glfw
)


add_executable(moonrock_output_png
    render_things.h             render_things.cpp
    output_png.cpp
)

target_link_libraries(moonrock_output_png
    moonrock
)
