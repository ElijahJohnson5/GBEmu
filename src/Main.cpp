#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!quit)
    {
        handleEvents(display, &quit);

        //PrintGBState(cpu, mmu);
        stepCPU(cpu, mmu);

        if (display->debugWindow->shown)
        {
            SDL_GL_MakeCurrent(display->debugWindow->window, display->glContext);
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(display->debugWindow->window);
            ImGui::NewFrame();
            {
                static float f = 0.0f;
                static int counter = 0;
                ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_Appearing);
                ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

                ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
                ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
                ImGui::Checkbox("Another Window", &show_another_window);

                ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

                if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                    counter++;
                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
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

/*
int DisassembleGB(unsigned char *codebuffer, int pc)
{
    unsigned char *code = &codebuffer[pc];
    //printf("%04X ", pc);
    #ifdef _DEBUG
    fprintf(debugFile, "%04X: ", pc);
    #else 
    printf("%04X: ", pc);
    #endif

    Instruction current = instructions[*code];
    if (current.execute == NULL)
    {
        #ifdef _DEBUG
        fprintf(debugFile, "Need to implement: %02X: %s", *code, current.disassembly);
        fprintf(debugFile, "\n");
        #else 
        printf("Need to implement: %02X: %s", *code, current.disassembly);
        printf("\n");
        #endif
        return current.operandCount + 1;
    }

    if (strcmp(current.disassembly, "PREFIX") == 0)
    {
        current = prefixInstructions[*(code + 1)];

        if (current.execute == NULL)
        {
            #ifdef _DEBUG
            fprintf(debugFile, "Need to implement: %02X: %s", *code, current.disassembly);
            fprintf(debugFile, "\n");
            #else 
            printf("Need to implement: %02X: %s", *code, current.disassembly);
            printf("\n");
            #endif
            return current.operandCount + 1;
        }

        ((ZeroOperands)current.execute)(current.disassembly);
    }
    else if (current.operandCount == 0)
    {
        ((ZeroOperands)current.execute)(current.disassembly);
    }
    else if (current.operandCount == 1)
    {
        ((OneOperands)current.execute)(current.disassembly, code[1]);
    }
    else if (current.operandCount == 2)
    {
        ((TwoOperands)current.execute)(current.disassembly, code[2], code[1]);
    }

    #ifdef _DEBUG
    fprintf(debugFile, "\n");
    #else 
    printf("\n");
    #endif

    return current.operandCount + 1;
}
*/