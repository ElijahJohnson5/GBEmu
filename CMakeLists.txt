cmake_minimum_required(VERSION 3.16)

project(Emulator)

find_package(SDL2 REQUIRED)

add_executable(Emulator
    src/CPU.c
    src/Instructions.c
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_impl_opengl3.cpp
    src/imgui/imgui_impl_sdl.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui.cpp
    src/glad/glad.c
    src/Memory.c
    src/Main.cpp
)

target_include_directories(Emulator PUBLIC include include/imgui include/glad ${SDL2_INCLUDE_DIRS})

target_compile_options(Emulator PUBLIC -Wall)
target_link_libraries(Emulator ${SDL2_LIBRARIES} dl m)