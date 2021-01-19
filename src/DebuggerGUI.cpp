#include "DebuggerGUI.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_memory_editor.h"

#include "CPU.h"
#include "Memory.h"
#include "Instructions.h"
#include "Main.h"

bool DebuggerGUI::paused = true;
bool DebuggerGUI::showMemoryViewer = false;
bool DebuggerGUI::showDissasembly = false;
MemoryEditor DebuggerGUI::memEdit;
Step DebuggerGUI::step = &stepCPU;

void DebuggerGUI::ShowDebuggerGUI(CPU* cpu, MMU* mmu, DisassembledInstruction* disassembledInstructions, int sizeInstructions)
{
    ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_Appearing);
    ImGui::Begin("Debug Data", NULL, ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if (ImGui::TreeNode("CPU"))
    {
        uint8_t op = readAddr8(mmu, cpu->pc);

        ImGui::Text("A: %X B: %X C: %X D: %X\n", cpu->a, cpu->b, cpu->c, cpu->d);
        ImGui::Text("E: %X F: %X H: %X L: %X\n", cpu->e, cpu->f, cpu->h, cpu->l);
        ImGui::Text("PC: %X SP: %X\n", cpu->pc, cpu->sp);
        ImGui::Text("Z: %X N: %X H: %X C: %X\n", cpu->cc.z, cpu->cc.n, cpu->cc.h, cpu->cc.c);
        ImGui::Text("Next instruction to execute: %X\n", op);
        Instruction current = instructions[op];

        if (strcmp(current.disassembly, "PREFIX") == 0)
        {
            current = prefixInstructions[readAddr8(mmu, cpu->pc + 1)];
            ImGui::Text(current.disassembly);
        }
        else if (current.operandCount == 0)
        {
            ImGui::Text(current.disassembly);
        }
        else if (current.operandCount == 1)
        {
            ImGui::Text(current.disassembly, readAddr8(mmu, cpu->pc + 1));
        }
        else if (current.operandCount == 2)
        {
            ImGui::Text(current.disassembly, readAddr8(mmu, cpu->pc + 2), readAddr8(mmu, cpu->pc + 1));
        }

        if (current.execute == NULL)
        {
            ImGui::Text("Need to implement this instructions");
        }
        

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNode("Windows"))
    {
        if (ImGui::Button("Memory Viewer"))
        {
            memEdit.OptGreyOutZeroes = false;
            memEdit.ReadOnly = true;
            memEdit.Cols = 8;
            showMemoryViewer = 1;
        }

        ImGui::SameLine();

        if (ImGui::Button("Disassembly"))
        {
            showDissasembly = 1;
        }
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNode("Debugging"))
    {
        if (!paused)
        {
            if (ImGui::Button("Pause"))
            {
                paused = !paused;
            }
        }
        else
        {
            if (ImGui::Button("Continue"))
            {
                paused = !paused;
            }

            ImGui::SameLine();
        }

        if (paused)
        {
            if (ImGui::Button("Step"))
            {
                if (step != nullptr)
                {
                    step(cpu, mmu);
                }
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset"))
        {
        }

        ImGui::TreePop();
    }
    ImGui::End();

    if (showMemoryViewer)
    {
        ShowMemoryViewerGUI(cpu, mmu);
    }

    if (showDissasembly)
    {
        ShowDisassemblyGUI(cpu, mmu, disassembledInstructions, sizeInstructions);
    }
}

void DebuggerGUI::ShowMemoryViewerGUI(CPU* cpu, MMU* mmu)
{
    ImGui::Begin("Memory Viewer", &showMemoryViewer, ImGuiWindowFlags_NoCollapse);
    memEdit.DrawContents(mmu->addr, 0x10000);
    ImGui::End();
}

void DebuggerGUI::ShowDisassemblyGUI(CPU* cpu, MMU* mmu, DisassembledInstruction* disassembledInstructions, int sizeInstructions)
{
    ImGui::Begin("Dissasembly", &showDissasembly, ImGuiWindowFlags_NoCollapse);
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), ImGuiWindowFlags_AlwaysVerticalScrollbar);
    ImGuiListClipper clipper(sizeInstructions, ImGui::GetTextLineHeightWithSpacing());
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            DisassembledInstruction &ins = disassembledInstructions[i];

            if (paused && cpu->pc == ins.pc)
            {
                ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), ins.disassembly);
            }
            else
            {
                ImGui::TextUnformatted(ins.disassembly);
            }


            ImGui::SameLine();
            ImGui::PushID(i);
            if (ins.breakpoint)
            {
                if (ImGui::Button("Disable Breakpoint"))
                {
                    ins.breakpoint = false;
                }
            }
            else
            {
                if (ImGui::Button("Breakpoint"))
                {
                    ins.breakpoint = true;
                }
            }
            ImGui::PopID();
        }
    }
    ImGui::EndChild();
    ImGui::End();
}
