set(target_name moonrock_simple)
set(src_dir ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS
    "${src_dir}/*.cpp"
)

file(GLOB_RECURSE private_headers CONFIGURE_DEPENDS
    "${src_dir}/*.h"
    "${src_dir}/*.hpp"
)


add_executable(${target_name})
set_target_properties(${target_name} PROPERTIES FOLDER "moonrock/examples")
target_compile_features(${target_name} PRIVATE cxx_std_20)

target_sources(${target_name}
    PUBLIC
        ${private_headers}
        ${source_files}
)

target_include_directories(${target_name}
    PUBLIC
        "${src_dir}"
)

target_link_libraries(${target_name}
    PUBLIC
        moonrock::moonrock
        SDL3::SDL3
)
