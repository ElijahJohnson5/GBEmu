#pragma once

typedef struct CPU CPU;
typedef struct MMU MMU;
typedef struct Video Video;
typedef struct MemoryEditor MemoryEditor;
typedef struct DisassembledInstruction DisassembledInstruction;

typedef void (*Step)(Video*, CPU*, MMU*);


class DebuggerGUI
{
public:
	static void ShowDebuggerGUI(Video* video, CPU* cpu, MMU* mmu, DisassembledInstruction* disassembledInstructions, int sizeInstructions);

	static bool paused;
	static bool showMemoryViewer;
	static bool showDissasembly;
	static MemoryEditor memEdit;
	static Step step;

private:
	static void ShowMemoryViewerGUI(CPU* cpu, MMU* mmu);
	static void ShowDisassemblyGUI(CPU* cpu, MMU* mmu, DisassembledInstruction*  disassembledInstructions, int sizeInstructions);
};

