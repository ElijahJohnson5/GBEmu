#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <map>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include <SDL.h>

#include "glad/glad.h"

#include "Main.h"
#include "Memory.h"
#include "CPU.h"
#include "Instructions.h"
#include "Display.h"
#include "DebuggerGUI.h"

int main(int argc, char *args[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Could not initialize SDL\n");
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    Display* display = createDisplay();

    if (!display) {
        printf("Could not create display\n");
        return -1;
    }

    SDL_GL_MakeCurrent(display->debugWindow->window, display->glContext);
    SDL_GL_SetSwapInterval(0);

    gladLoadGL();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(display->debugWindow->window, display->glContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    FILE* gbFile = fopen("roms/Tetris.gb", "rb");

    if (!gbFile)
    {
        printf("Could not open file \"roms/Tetris.gb\"\n");
        return -1;
    }

    fseek(gbFile, 0, SEEK_END);
    long size = ftell(gbFile);
    rewind(gbFile);

    unsigned char *file = (unsigned char*)malloc(size + 1);
    fread(file, 1, size, gbFile);

    fclose(gbFile);

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
        destroyDisplay(display);
        free(file);
        return -1;
    }

    CPU* cpu = createCPU();

    memcpy(mmu->rom, file, size);

    free(file);

    loadBios(mmu);
    resetCPU(cpu);
    loadGame(cpu, mmu);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!quit)
    {
        handleEvents(display, &quit, &ImGui_ImplSDL2_ProcessEvent);

        //PrintGBState(cpu, mmu);
        if (!DebuggerGUI::paused)
        {
            stepCPU(cpu, mmu);
            if (disassembledInstructions[pcToIndex[cpu->pc]].breakpoint)
            {
                DebuggerGUI::paused = true;
            }
        }

        if (display->debugWindow->shown)
        {
            SDL_GL_MakeCurrent(display->debugWindow->window, display->glContext);
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(display->debugWindow->window);
            ImGui::NewFrame();
            {
                DebuggerGUI::ShowDebuggerGUI(cpu, mmu, disassembledInstructions, i);
            }
            ImGui::Render();
            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(display->debugWindow->window);
        }
        
        updateMainWindow(display);
        //getchar();
    }

    delete[] disassembledInstructions;

    destroyMMU(mmu);
    destroyCPU(cpu);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    destroyDisplay(display);

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

    ins.disassembly = new char[instru.str().length()];
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
        printf(current.disassembly);
    }
    else if (current.operandCount == 0)
    {
        printf(current.disassembly);
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