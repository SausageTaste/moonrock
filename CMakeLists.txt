cmake_minimum_required(VERSION 3.20)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

project(moonrock
    VERSION 0.1.0
    LANGUAGES CXX
)


find_package(glm CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)


add_subdirectory(lib)
add_subdirectory(example)
