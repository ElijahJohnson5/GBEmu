cmake_minimum_required(VERSION 3.16)

project(Emulator)

if (WIN32)
    set(SDL2_DIR "${CMAKE_CURRENT_LIST_DIR}/SDL2-2.0.14/")
endif()

find_package(SDL2 REQUIRED)

add_executable(Emulator
    src/CPU.c
    include/CPU.h
    src/Instructions.c
    include/Instructions.h
    src/Memory.c
    include/Memory.h
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_impl_opengl3.cpp
    include/imgui/imgui_impl_opengl3.h
    src/imgui/imgui_impl_sdl.cpp
    include/imgui/imgui_impl_sdl.h
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui.cpp
    include/imgui/imgui.h
    include/imgui/imgui_internal.h
    include/imgui/imstb_rectpack.h
    include/imgui/imstb_textedit.h
    include/imgui/imstb_truetype.h
    src/glad/glad.c
    include/glad/glad.h
    include/KHR/khrplatform.h
    src/Main.cpp
    include/Main.h
)

target_include_directories(Emulator PUBLIC include include/imgui include/glad ${SDL2_INCLUDE_DIRS})

target_compile_options(Emulator PUBLIC -Wall)

if (NOT WIN32)
    target_link_libraries(Emulator dl m)
else ()
    add_custom_command(TARGET Emulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL2_DLL}" $<TARGET_FILE_DIR:Emulator>
    )
endif()

target_link_libraries(Emulator ${SDL2_LIBRARIES})