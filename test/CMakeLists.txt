
add_executable(moonrock_output_window
    output_window.cpp
)

target_link_libraries(moonrock_output_window
    moonrock
    SDL2main
    SDL2-static
)
