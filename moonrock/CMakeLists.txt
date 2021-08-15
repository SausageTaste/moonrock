
add_library(moonrock
    moonrock.h                  moonrock.cpp
)

target_include_directories(moonrock INTERFACE
    ${PROJECT_SOURCE_DIR}
)
