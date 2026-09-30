include(FetchContent)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity
    GIT_TAG v2.6.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(unity)

add_subdirectory(unit)
