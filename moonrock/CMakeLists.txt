
add_library(moonrock
    moonrock.h                  moonrock.cpp
)
target_compile_features(moonrock PUBLIC cxx_std_17)

target_include_directories(moonrock
    INTERFACE
        ${PROJECT_SOURCE_DIR}
    PRIVATE
        ${fetch_stb_SOURCE_DIR}
)
