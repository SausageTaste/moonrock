
add_library(moonrock
    moonrock.h                  moonrock.cpp
    math_tool.h                 math_tool.cpp
    mesh.h                      mesh.cpp
    utils.h                     utils.cpp
    image.h                     image.cpp
    actor.h                     actor.cpp
)

target_compile_features(moonrock PUBLIC cxx_std_17)

target_include_directories(moonrock
    INTERFACE
        ${PROJECT_SOURCE_DIR}
    PRIVATE
        ${fetch_stb_SOURCE_DIR}
)

target_link_libraries(moonrock
    PUBLIC
        glm::glm
    PRIVATE
        dalbaragi::daltools
)
