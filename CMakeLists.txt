cmake_minimum_required(VERSION 3.16)

project(Emulator)
if (APPLE)
    set (CMAKE_CXX_STANDARD 11)
endif()


set(SRC 
    src/CPU.c
    src/Instructions.c
    src/Memory.c
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_impl_opengl3.cpp
    src/imgui/imgui_impl_sdl.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui.cpp
    src/glad/glad.c
    src/Main.cpp
    src/Window.c
    src/Video.c
    src/Util.c
    src/Shader.c
    src/DebuggerGUI.cpp
)

if (WIN32)
    set(SDL2_DIR "${CMAKE_CURRENT_LIST_DIR}/SDL2-2.0.14/")
    #Add h files for visual studio don't need these on linux
    list(APPEND SRC 
        include/CPU.h
        include/Instructions.h
        include/Memory.h
        include/imgui/imgui_impl_opengl3.h
        include/imgui/imgui_impl_sdl.h
        include/imgui/imgui.h
        include/imgui/imgui_internal.h
        include/imgui/imstb_rectpack.h
        include/imgui/imstb_textedit.h
        include/imgui/imstb_truetype.h
        include/Main.h
        include/glad/glad.h
        include/KHR/khrplatform.h
        include/Window.h
        include/Video.h
        include/Shader.h
        include/Util.h
        include/DebuggerGUI.h
        include/imgui/imgui_memory_editor.h
        include/imgui/imconfig.h
    )
endif()

find_package(SDL2 REQUIRED)

add_executable(Emulator
    ${SRC}
)

target_include_directories(Emulator PUBLIC include include/imgui include/glad ${SDL2_INCLUDE_DIRS})

if (NOT WIN32)
    target_link_libraries(Emulator dl m pthread)
    target_compile_options(Emulator PUBLIC -g -Wall)
else ()
    add_custom_command(TARGET Emulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL2_DLL}" $<TARGET_FILE_DIR:Emulator>
    )
endif()

target_link_libraries(Emulator ${SDL2_LIBRARIES})