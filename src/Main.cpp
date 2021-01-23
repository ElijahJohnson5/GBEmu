#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <thread>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imconfig.h"
#include <SDL.h>

#include "glad/glad.h"

#include "Main.h"
#include "Instructions.h"
#include "DebuggerGUI.h"
#include "Util.h"
#include "Shader.h"

int main(int argc, char *args[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Could not initialize SDL\n");
        return -1;
    }

    #ifdef __APPLE__ 
        const char* glsl_version = "#version 150";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    #else
        const char* glsl_version = "#version 130";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    Window* mainWindow = createWindow("GBEmu", 1280, 720, 1);

    if (!mainWindow) {
        printf("Could not create mainWindow\n");
        return -1;
    }

    SDL_GL_MakeCurrent(mainWindow->window, mainWindow->glContext);
    SDL_GL_SetSwapInterval(0);

    Window* debugWindow = createWindow("GBEmu Debugger", 1280, 720, 0);

    if (!mainWindow) {
        printf("Could not create debugWindow\n");
        return -1;
    }

    SDL_GL_MakeCurrent(debugWindow->window, debugWindow->glContext);
    SDL_GL_SetSwapInterval(1);

    gladLoadGL();


    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(debugWindow->window, debugWindow->glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    long size;
    unsigned char* file = (unsigned char*)readFile("roms/Tetris.gb", "rb", &size);

    int pc = 0;
    int i = 0;
    DisassembledInstruction* disassembledInstructions = new DisassembledInstruction[0x7FFF];
    std::map<int, int> pcToIndex;

    while (pc < size)
    {
        pcToIndex[pc] = i;
        disassembledInstructions[i++] = disassembleInstructions(file, &pc);
    }


    int quit = 0;

    MMU* mmu = createMMU();

    if (!mmu)
    {
        printf("Could not create mmu\n");
        destroyWindow(mainWindow);
        destroyWindow(debugWindow);
        free(file);
        return -1;
    }

    CPU* cpu = createCPU();
    Video* video = createVideo(mmu);

    //TODO move into some kind of drawing for the mainwindow
    SDL_GL_MakeCurrent(mainWindow->window, mainWindow->glContext);

    uint32_t shaderID = createShaderProgram("shader/texture.vs", "shader/texture.fs");

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint mainTextureID;
    glGenTextures(1, &mainTextureID);
    glBindTexture(GL_TEXTURE_2D, mainTextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VIDEO_WIDTH, VIDEO_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    memcpy(mmu->rom, file, size);

    free(file);

    loadBios(mmu);
    resetCPU(cpu);
    loadGame(cpu, mmu);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::thread cpuThread(CPUThread, std::ref(quit), cpu, mmu, video, disassembledInstructions, pcToIndex);

    while (!quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type)
            {
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_WINDOWEVENT:
                    //TODO make the windows handle their own events
                    switch (event.window.event)
                    {
                        case SDL_WINDOWEVENT_CLOSE:
                            if (event.window.windowID == mainWindow->id)
                            {
                                quit = 1;
                            }
                            else if (debugWindow->shown && event.window.windowID == debugWindow->id)
                            {
                                debugWindow->shown = 0;
                                SDL_HideWindow(debugWindow->window);
                            }
                            break;
                        case SDL_WINDOWEVENT_RESIZED:
                            if (event.window.windowID == mainWindow->id)
                            {
                                mainWindow->width = event.window.data1;
                                mainWindow->height = event.window.data2;
                            }
                            if (event.window.windowID == debugWindow->id)
                            {
                                debugWindow->width = event.window.data1;
                                debugWindow->height = event.window.data2;
                            }
                            break;
                    }
                    break;
            }
        }

        //TODO make this modifiable in config files
        const uint8_t* keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_LCTRL] && keys[SDL_SCANCODE_B])
        {
            if (debugWindow->shown == 0)
            {
                SDL_ShowWindow(debugWindow->window);
                debugWindow->shown = 1;
            }
        }


        if (debugWindow->shown)
        {
            SDL_GL_MakeCurrent(debugWindow->window, debugWindow->glContext);
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(debugWindow->window);
            ImGui::NewFrame();
            {
                DebuggerGUI::ShowDebuggerGUI(video, cpu, mmu, disassembledInstructions, i);
            }
            ImGui::Render();
            glViewport(0, 0, (int)debugWindow->width, (int)debugWindow->height);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(debugWindow->window);
        }


       // if (video->canrender)
       // {
            SDL_GL_MakeCurrent(mainWindow->window, mainWindow->glContext);
            glClear(GL_COLOR_BUFFER_BIT);
            glBindTexture(GL_TEXTURE_2D, mainTextureID);
            //TODO figure out off by one in video_height somewhere
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, VIDEO_WIDTH, VIDEO_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, video->framebuffer);

            glUseProgram(shaderID);
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindTexture(GL_TEXTURE_2D, 0);
            video->canrender = 0;
            SDL_GL_SwapWindow(mainWindow->window);
       // }
    }

    cpuThread.join();


    for (int j = 0; j < i; j++) {
        delete[] disassembledInstructions[i].disassembly;
    }

    delete[] disassembledInstructions;

    destroyVideo(video);
    destroyCPU(cpu);
    destroyMMU(mmu);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    destroyWindow(mainWindow);

    SDL_Quit();

    return 0;
}

DisassembledInstruction disassembleInstructions(unsigned char* codebuffer, int *pc)
{
    unsigned char* code = &codebuffer[*pc];

    DisassembledInstruction ins;
    ins.pc = *pc;
    ins.breakpoint = false;

    std::stringstream instru;

    instru << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << *pc << ": ";

    Instruction current = instructions[*code];

    if (strcmp(current.disassembly, "PREFIX") == 0)
    {
        current = prefixInstructions[*(code + 1)];

        instru << std::string(current.disassembly);
        (*pc) += 2;
    }
    else if (current.operandCount == 0)
    {
        instru << std::string(current.disassembly);
        (*pc)++;
    }
    else if (current.operandCount == 1)
    {
        char buf[1024];
        sprintf(buf, current.disassembly, code[1]);
        instru << std::string(buf);
        (*pc) += 2;
    }
    else if (current.operandCount == 2)
    {
        char buf[1024];
        sprintf(buf, current.disassembly, code[2], code[1]);
        instru << std::string(buf);
        (*pc) += 3;
    }

    ins.disassembly = new char[instru.str().length() + 1];
    strcpy(ins.disassembly, instru.str().c_str());

    return ins;
}

void PrintGBState(CPU *cpu, MMU* mmu)
{
    uint8_t op = readAddr8(mmu, cpu->pc);
    printf("A: %X B: %X C: %X D: %X\n", cpu->a, cpu->b, cpu->c, cpu->d);
    printf("E: %X F: %X H: %X L: %X\n", cpu->e, cpu->f, cpu->h, cpu->l);
    printf("PC: %X SP: %X\n", cpu->pc, cpu->sp);
    printf("Z: %X N: %X H: %X C: %X\n", cpu->cc.z, cpu->cc.n, cpu->cc.h, cpu->cc.c);
    printf("Next instruction to execute: %X\n", op);
    printf("Dissasembled instruction: \n");

    Instruction current = instructions[op];

    if (strcmp(current.disassembly, "PREFIX") == 0)
    {
        current = prefixInstructions[readAddr8(mmu, cpu->pc + 1)];
        printf("%s", current.disassembly);
    }
    else if (current.operandCount == 0)
    {
        printf("%s", current.disassembly);
    }
    else if (current.operandCount == 1)
    {
        printf(current.disassembly, readAddr8(mmu, cpu->pc + 1));
    }
    else if (current.operandCount == 2)
    {
        printf(current.disassembly, readAddr8(mmu, cpu->pc + 2), readAddr8(mmu, cpu->pc + 1));
    }
    printf("\n");
}

void CPUThread(int& quit, CPU* cpu, MMU* mmu, Video* video, DisassembledInstruction* disassembledInstructions, std::map<int, int> pcToIndex)
{
    while (!quit)
    {
        if (!DebuggerGUI::paused)
        {
            stepCPU(cpu, mmu);
            stepVideo(video, cpu, mmu);
            if (disassembledInstructions[pcToIndex[cpu->pc]].breakpoint)
            {
                DebuggerGUI::paused = true;
            }
        }
        SDL_Delay(0);
    }
}
