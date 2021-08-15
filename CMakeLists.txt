cmake_minimum_required(VERSION 3.11)

include(FetchContent)

project(moonrock
    LANGUAGES CXX
)


FetchContent_Declare(
    fetch_moonrock_stb
    GIT_REPOSITORY https://github.com/nothings/stb
)

FetchContent_Declare(
    fetch_moonrock_glm
    GIT_REPOSITORY https://github.com/g-truc/glm
)

FetchContent_MakeAvailable(fetch_moonrock_stb)
FetchContent_MakeAvailable(fetch_moonrock_glm)


add_subdirectory(moonrock)

add_subdirectory(test)
