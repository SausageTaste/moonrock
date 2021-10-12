cmake_minimum_required(VERSION 3.11)

include(FetchContent)

project(moonrock
    LANGUAGES CXX
)


FetchContent_Declare(
    fetch_stb
    GIT_REPOSITORY https://github.com/nothings/stb
)

FetchContent_Declare(
    fetch_glm
    GIT_REPOSITORY https://github.com/g-truc/glm
)

FetchContent_Declare(
    fetch_glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
)

FetchContent_Declare(
    fetch_glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad
)

FetchContent_Declare(
    fetch_daltools
    GIT_REPOSITORY https://github.com/SausageTaste/DalbaragiTools
    GIT_TAG        main
)

FetchContent_MakeAvailable(fetch_stb)
FetchContent_MakeAvailable(fetch_glm)
FetchContent_MakeAvailable(fetch_glfw)
FetchContent_MakeAvailable(fetch_glad)
FetchContent_MakeAvailable(fetch_daltools)


add_subdirectory(moonrock)
add_subdirectory(test)
