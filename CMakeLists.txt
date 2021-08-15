cmake_minimum_required(VERSION 3.11)

include(FetchContent)

project(moonrock
    LANGUAGES CXX
)


FetchContent_Declare(
    fetch_stb
    GIT_REPOSITORY https://github.com/nothings/stb
)

FetchContent_MakeAvailable(fetch_stb)


add_subdirectory(moonrock)

add_subdirectory(test)
