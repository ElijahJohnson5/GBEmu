#include "Instructions.h"
#include "CPU.h"
#include "Memory.h"

void NOP(CPU* cpu, MMU* mmu)
{
    return;
}

void DI(CPU* cpu, MMU* mmu)
{
    return;
}

void JP_NN(CPU* cpu, MMU* mmu)
{
    uint16_t addr = readAddr16(mmu, cpu->pc);
    JP(cpu, addr, 0x02);
}

void XOR(CPU* cpu, uint8_t value)
{
    cpu->a ^= value;

    cpu->cc.z = !cpu->a;

    cpu->cc.c = 0;
    cpu->cc.h = 0;
    cpu->cc.n = 0;
}

uint8_t DEC8(CPU* cpu, uint8_t value)
{
    value--;

    cpu->cc.n = 1;
    cpu->cc.h = (value & 0x0F) == 0x0F;
    cpu->cc.z = value == 0;

    return value;
}

void XOR_A_A(CPU* cpu, MMU* mmu)
{
    XOR(cpu, cpu->a);
}

void LD_HL_NN(CPU* cpu, MMU* mmu)
{
    uint16_t value = readAddr16(mmu, cpu->pc);
    cpu->hl = value;
}

void LD_C_N(CPU* cpu, MMU* mmu)
{
    uint8_t value = readAddr8(mmu, cpu->pc);
    cpu->c = value;
}

void LD_B_N(CPU* cpu, MMU* mmu)
{
    uint8_t value = readAddr8(mmu, cpu->pc);
    cpu->b = value;
}

void LD_A_N(CPU* cpu, MMU* mmu)
{
    uint8_t value = readAddr8(mmu, cpu->pc);
    cpu->a = value;
}

void LD_HLDEC_A(CPU* cpu, MMU* mmu)
{
    writeAddr8(mmu, cpu->hl, cpu->a);
    cpu->hl--;
}


void LD_FF00_A(CPU* cpu, MMU* mmu)
{
    uint8_t value = readAddr8(mmu, cpu->pc);
    writeAddr8(mmu, 0xFF00 + value, cpu->a);
}

void DEC_B(CPU* cpu, MMU* mmu)
{
    cpu->b = DEC8(cpu, cpu->b);
}

void DEC_C(CPU* cpu, MMU* mmu)
{
    cpu->c = DEC8(cpu, cpu->c);
}

void JR_NZ_N(CPU* cpu, MMU* mmu)
{
    if (cpu->cc.z)
    {
        return;
    }
    else 
    {
        int8_t offset = (int8_t)readAddr8(mmu, cpu->pc);
        JR(cpu, offset);
    }
}

void JR(CPU *cpu, int8_t offset)
{
    cpu->pc += offset;
    cpu->currentClock += 4;
}

void JP(CPU* cpu, uint16_t addr, uint8_t offset)
{
    cpu->pc = addr - offset;
    cpu->currentClock += 4;
}

const Instruction instructions[256] = {
    {"NOP", 0, 4, &NOP}, // case 0x00: fprintf(debugFile, "NOP"); printf("NOP "); break;
    {"LD BC,$%02X%02X", 2, 12, NULL}, // case 0x01: printf("LD BC,%02X%02X", code[2], code[1]); opbytes = 3; break;
    {"LD (BC),A", 0, 8, NULL}, // case 0x02: printf("LD (BC),A "); break;
    {"INC BC", 0, 8, NULL},// case 0x03: printf("INC BC "); break;
    {"INC B", 0, 4, NULL},// case 0x04: printf("INC B "); break;
    {"DEC B", 0, 4, &DEC_B},// case 0x05: printf("DEC B "); break;
    {"LD B,$%02X", 1, 8, &LD_B_N},// case 0x06: printf("LD B,%02X ", code[1]); opbytes = 2; break;
    {"RLCA", 0, 4, NULL},// case 0x07: printf("RLCA"); break;
    {"LD $%02X%02X,SP", 2, 20, NULL},// case 0x08: printf("LD %02X%02X,SP ", code[2], code[1]); opbytes = 3; break;
    {"ADD HL,BC", 0, 8, NULL},// case 0x09: printf("ADD HL,BC "); break;
    {"LD A,(BC)", 0, 8, NULL},// case 0x0A: printf("LD A,(BC) "); break;
    {"DEC BC", 0, 8, NULL},// case 0x0B: printf("DEC BC"); break;
    {"INC C", 0, 4, NULL},// case 0x0C: printf("INC C "); break;
    {"DEC C", 0, 4, &DEC_C},// case 0x0D: printf("DEC C "); break;
    {"LD C,$%02X", 1, 8, &LD_C_N},// case 0x0E: printf("LD C,%02X ", code[1]); opbytes = 2; break;
    {"RRCA", 0, 4, NULL},// case 0x0F: printf("RRCA"); break;

    {"STOP $%02X", 1, 4, NULL}, // case 0x10: printf("STOP 0"); opbytes = 2; break;
    {"LD DE,$%02X%02X", 2, 12, NULL},// case 0x11: printf("LD DE,%02X%02X ", code[2], code[1]); opbytes = 3; break;
    {"LD (DE),A", 0, 8, NULL},// case 0x12: printf("LD (DE),A "); break;
    {"INC DE", 0, 8, NULL},// case 0x13: printf("INC DE "); break;
    {"IND D", 0, 4, NULL},// case 0x14: printf("INC D "); break;
    {"DEC D", 0, 4, NULL},// case 0x15: printf("DEC D "); break;
    {"LD D,$%02X", 1, 8, NULL},// case 0x16: printf("LD D,%02X ", code[1]); opbytes = 2; break;
    {"RLA", 0, 4, NULL},// case 0x17: printf("RLA "); break;
    {"JR $%02X", 1, 12, NULL},// case 0x18: printf("JR %02X", code[1]); opbytes = 2; break;
    {"ADD HL,DE", 0, 8, NULL},// case 0x19: printf("ADD HL,DE"); break;
    {"LD A,(DE)", 0, 8, NULL},// case 0x1A: printf("LD A,(DE)"); break;
    {"DEC DE", 0, 8, NULL},// case 0x1B: printf("DEC DE"); break;
    {"INC E", 0, 4, NULL},// case 0x1C: printf("INC E"); break;
    {"DEC E", 0, 4, NULL},// case 0x1D: printf("DEC E"); break;
    {"LD E,$%02X", 1, 8, NULL},// case 0x1E: printf("LD E,%02X", code[1]); opbytes = 2; break;
    {"RRA", 0, 4, NULL},// case 0x1F: printf("RRA"); break;

    {"JR NZ,$%02X", 1, 8, &JR_NZ_N},// case 0x20: printf("JR NZ,%02X", code[1]); opbytes = 2; break;
    {"LD HL,$%02X%02X", 2, 12, &LD_HL_NN}, //case 0x21: printf("LD HL,%02X%02X", code[2], code[1]); opbytes = 3; break;
    {"LD (HL+),A", 0, 8, NULL},// case 0x22: printf("LD (HL+),A"); break;
    {"INC HL", 0, 8, NULL},// case 0x23: printf("INC HL"); break;
    {"INC H", 0, 4, NULL},// case 0x24: printf("INC H"); break;
    {"DEC H", 0, 4, NULL},// case 0x25: printf("DEC H"); break;
    {"LD H,$%02X", 1, 8, NULL},// case 0x26: printf("LD H,%02X", code[1]); opbytes = 2; break;
    {"DAA", 0, 4, NULL},// case 0x27: printf("DAA"); break;
    {"JR Z,$%02X", 1, 8, NULL},// case 0x28: printf("JR Z,%02X", code[1]); opbytes = 2; break; 
    {"ADD HL,HL", 0, 8, NULL},// case 0x29: printf("ADD HL,HL"); break;
    {"LD A,(HL+)", 0, 8, NULL},// case 0x2A: printf("LD A,(HL+)"); break;
    {"DEC HL", 0, 8, NULL},// case 0x2B: printf("DEC HL"); break;
    {"INC L", 0, 4, NULL},// case 0x2C: printf("INC L"); break;
    {"DEC L", 0, 4, NULL},// case 0x2D: printf("DEC L"); break;
    {"LD L,$%02X", 1, 8, NULL},// case 0x2E: printf("LD L,%02X", code[1]); opbytes = 2; break;
    {"CPL", 0, 4, NULL},// case 0x2F: printf("CPL"); break;

    {"JR NC,$%02X", 1, 8, NULL},// case 0x30: printf("JR NC,%02X", code[1]); opbytes = 2; break;
    {"LD SP,$%02X%02X", 2, 12, NULL},// case 0x31: printf("LD SP,%02X%02X", code[2], code[1]); opbytes = 3; break;
    {"LD (HL-),A", 0, 8, &LD_HLDEC_A},// case 0x32: printf("LD (HL-),A"); break;
    {"INC SP", 0, 8, NULL},// case 0x33: printf("INC SP"); break;
    {"INC (HL)", 0, 12, NULL},// case 0x34: printf("INC (HL)"); break;
    {"DEC (HL)", 0, 12, NULL},// case 0x35: printf("DEC (HL)"); break;
    {"LD (HL),$%02X", 1, 12, NULL},// case 0x36: printf("LD (HL),%02X", code[1]); opbytes = 2; break;
    {"SCF", 0, 4, NULL},// case 0x37: printf("SCF"); break;
    {"JR C,$%02X", 1, 8, NULL},// case 0x38: printf("JR C,%02X", code[1]); opbytes = 2; break; 
    {"ADD HL,SP", 0, 8, NULL},// case 0x39: printf("ADD HL,SP"); break; 
    {"LD A,(HL-)", 0, 8, NULL},// case 0x3A: printf("LD A,(HL-)"); break;
    {"DEC SP", 0, 8, NULL},// case 0x3B: printf("DEC SP"); break;
    {"INC A", 0, 4, NULL},// case 0x3C: printf("INC A"); break;
    {"DEC A", 0, 4, NULL},// case 0x3D: printf("DEC A"); break;
    {"LD A,$%02X", 1, 8, &LD_A_N},// case 0x3E: printf("LD A,%02X", code[1]); opbytes = 2; break;
    {"CCF", 0, 4, NULL},// case 0x3F: printf("CCF"); break;

    {"LD B,B", 0, 4, NULL},// case 0x40: printf("LD B,B "); break;
    {"LD B,C", 0, 4, NULL},// case 0x41: printf("LD B,C "); break;
    {"LD B,D", 0, 4, NULL},// case 0x42: printf("LD B,D "); break;
    {"LD B,E", 0, 4, NULL},// case 0x43: printf("LD B,E "); break;
    {"LD B,H", 0, 4, NULL},// case 0x44: printf("LD B,H "); break;
    {"LD B,L", 0, 4, NULL},// case 0x45: printf("LD B,L "); break;
    {"LD B,(HL)", 0, 8, NULL},// case 0x46: printf("LD B,(HL) "); break;
    {"LD B,A", 0, 4, NULL},// case 0x47: printf("LD B,A");
    {"LD C,B", 0, 4, NULL},// case 0x48: printf("LD C,B "); break;
    {"LD C,C", 0, 4, NULL},// case 0x49: printf("LD C,C "); break;
    {"LD C,D", 0, 4, NULL},// case 0x4A: printf("LD C,D "); break;
    {"LD C,E", 0, 4, NULL},// case 0x4B: printf("LD C,E "); break;
    {"LD C,H", 0, 4, NULL},// case 0x4C: printf("LD C,H "); break;
    {"LD C,L", 0, 4, NULL},// case 0x4D: printf("LD C,L "); break;
    {"LD C,(HL)", 0, 8, NULL},// case 0x4E: printf("LD C,(HL) "); break;
    {"LD C,A", 0, 4, NULL},// case 0x4F: printf("LD C,A"); break;

    {"LD D,B", 0, 4, NULL},// case 0x50: printf("LD D,B "); break;
    {"LD D,C", 0, 4, NULL},// case 0x51: printf("LD D,C "); break;
    {"LD D,D", 0, 4, NULL},// case 0x52: printf("LD D,D "); break;
    {"LD D,E", 0, 4, NULL},// case 0x53: printf("LD D,E "); break;
    {"LD D,H", 0, 4, NULL},// case 0x54: printf("LD D,H "); break;
    {"LD D,L", 0, 4, NULL},// case 0x55: printf("LD D,L "); break;
    {"LD D,(HL)", 0, 8, NULL},// case 0x56: printf("LD D,(HL) "); break;
    {"LD D,A", 0, 4, NULL},// case 0x57: printf("LD D,A"); break;
    {"LD E,B", 0, 4, NULL},// case 0x58: printf("LD E,B"); break;
    {"LD E,C", 0, 4, NULL},// case 0x59: printf("LD E,C "); break;
    {"LD E,D", 0, 4, NULL},// case 0x5A: printf("LD E,D "); break;
    {"LD E,E", 0, 4, NULL},// case 0x5B: printf("LD E,E "); break;
    {"LD E,H", 0, 4, NULL},// case 0x5C: printf("LD E,H "); break;
    {"LD E,L", 0, 4, NULL},// case 0x5D: printf("LD E,L "); break;
    {"LD E,(HL)", 0, 8, NULL},// case 0x5E: printf("LD E,(HL) "); break;
    {"LD E,A", 0, 4, NULL},// case 0x5F: printf("LD E,A"); break;

    {"LD H,B", 0, 4, NULL},// case 0x60: printf("LD H,B"); break;
    {"LD H,C", 0, 4, NULL},// case 0x61: printf("LD H,C "); break;
    {"LD H,D", 0, 4, NULL},// case 0x62: printf("LD H,D "); break;
    {"LD H,E", 0, 4, NULL},// case 0x63: printf("LD H,E "); break;
    {"LD H,H", 0, 4, NULL},// case 0x64: printf("LD H,H "); break;
    {"LD H,L", 0, 4, NULL},// case 0x65: printf("LD H,L "); break;
    {"LD H,(HL)", 0, 8, NULL},// case 0x66: printf("LD H,(HL) "); break;
    {"LD H,A", 0, 4, NULL},// case 0x67: printf("LD H,A"); break;
    {"LD L,B", 0, 4, NULL},// case 0x68: printf("LD L,B"); break;
    {"LD L,C", 0, 4, NULL},// case 0x69: printf("LD L,C "); break;
    {"LD L,D", 0, 4, NULL},// case 0x6A: printf("LD L,D "); break;
    {"LD L,E", 0, 4, NULL},// case 0x6B: printf("LD L,E "); break;
    {"LD L,H", 0, 4, NULL},// case 0x6C: printf("LD L,H "); break;
    {"LD L,L", 0, 4, NULL},// case 0x6D: printf("LD L,L "); break;
    {"LD L,(HL)", 0, 8, NULL},// case 0x6E: printf("LD L,(HL) "); break;
    {"LD L,A", 0, 4, NULL},// case 0x6F: printf("LD L,A"); break;

    {"LD (HL),B", 0, 4, NULL},// case 0x70: printf("LD (HL),B"); break;
    {"LD (HL),C", 0, 4, NULL},// case 0x71: printf("LD (HL),C "); break;
    {"LD (HL),D", 0, 4, NULL},// case 0x72: printf("LD (HL),D "); break;
    {"LD (HL),E", 0, 4, NULL},// case 0x73: printf("LD (HL),E "); break;
    {"LD (HL),H", 0, 4, NULL},// case 0x74: printf("LD (HL),H "); break;
    {"LD (HL),L", 0, 4, NULL},// case 0x75: printf("LD (HL),L "); break;
    {"HALT", 0, 4, NULL},// case 0x76: printf("HALT"); break;
    {"LD (HL),A", 0, 8, NULL},// case 0x77: printf("LD (HL),A"); break;
    {"LD A,B", 0, 4, NULL},// case 0x78: printf("LD A,B"); break;
    {"LD A,C", 0, 4, NULL},// case 0x79: printf("LD A,C "); break;
    {"LD A,D", 0, 4, NULL},// case 0x7A: printf("LD A,D "); break;
    {"LD A,E", 0, 4, NULL},// case 0x7B: printf("LD A,E "); break;
    {"LD A,H", 0, 4, NULL},// case 0x7C: printf("LD A,H "); break;
    {"LD A,L", 0, 4, NULL},// case 0x7D: printf("LD A,L "); break;
    {"LD A,(HL)", 0, 8, NULL},// case 0x7E: printf("LD A,(HL) "); break;
    {"LD A,A", 0, 4, NULL},// case 0x7F: printf("LD A,A "); break;

    // // Add n
    {"ADD A,B", 0, 4, NULL},// case 0x80: printf("ADD A,B"); break;
    {"ADD A,C", 0, 4, NULL},// case 0x81: printf("ADD A,C"); break;
    {"ADD A,D", 0, 4, NULL},// case 0x82: printf("ADD A,D"); break;
    {"ADD A,E", 0, 4, NULL},// case 0x83: printf("ADD A,E"); break;
    {"ADD A,H", 0, 4, NULL},// case 0x84: printf("ADD A,H"); break;
    {"ADD A,L", 0, 4, NULL},// case 0x85: printf("ADD A,L"); break;
    {"ADD A,(HL)", 0, 8, NULL},// case 0x86: printf("ADD A,(HL)"); break;
    {"ADD A,A", 0, 4, NULL},// case 0x87: printf("ADD A,A"); break;
    // // Add n + carry flag
    {"ADC A,B", 0, 4, NULL},// case 0x88: printf("ADC A,B"); break;
    {"ADC A,C", 0, 4, NULL},// case 0x89: printf("ADC A,C"); break;
    {"ADC A,D", 0, 4, NULL},// case 0x8A: printf("ADC A,D"); break;
    {"ADC A,E", 0, 4, NULL},// case 0x8B: printf("ADC A,E"); break;
    {"ADC A,H", 0, 4, NULL},// case 0x8C: printf("ADC A,H"); break;
    {"ADC A,L", 0, 4, NULL},// case 0x8D: printf("ADC A,L"); break;
    {"ADC A,(HL)", 0, 8, NULL},// case 0x8E: printf("ADC A,(HL)"); break;
    {"ADC A,A", 0, 4, NULL},// case 0x8F: printf("ADC A,A"); break;

    {"SUB A,B", 0, 4, NULL},// case 0x90: printf("SUB A,B"); break;
    {"SUB A,C", 0, 4, NULL},// case 0x91: printf("SUB A,C"); break;
    {"SUB A,D", 0, 4, NULL},// case 0x92: printf("SUB A,D"); break;
    {"SUB A,E", 0, 4, NULL},// case 0x93: printf("SUB A,E"); break;
    {"SUB A,H", 0, 4, NULL},// case 0x94: printf("SUB A,H"); break;
    {"SUB A,L", 0, 4, NULL},// case 0x95: printf("SUB A,L"); break;
    {"SUB A,(HL)", 0, 8, NULL},// case 0x96: printf("SUB A,(HL)"); break;
    {"SUB A,A", 0, 4, NULL},// case 0x97: printf("SUB A,A"); break;
    {"SBC A,B", 0, 4, NULL},// case 0x98: printf("SBC A,B"); break;
    {"SBC A,C", 0, 4, NULL},// case 0x99: printf("SBC A,C"); break;
    {"SBC A,D", 0, 4, NULL},// case 0x9A: printf("SBC A,D"); break;
    {"SBC A,E", 0, 4, NULL},// case 0x9B: printf("SBC A,E"); break;
    {"SBC A,H", 0, 4, NULL},// case 0x9C: printf("SBC A,H"); break;
    {"SBC A,L", 0, 4, NULL},// case 0x9D: printf("SBC A,L"); break;
    {"SBC A,(HL)", 0, 8, NULL},// case 0x9E: printf("SBC A,(HL)"); break;
    {"SBC A,A", 0, 4, NULL},// case 0x9F: printf("SBC A,A"); break;

    {"AND A,B", 0, 4, NULL},// case 0xA0: printf("AND A,B"); break;
    {"AND A,C", 0, 4, NULL},// case 0xA1: printf("AND A,C"); break;
    {"AND A,D", 0, 4, NULL},// case 0xA2: printf("AND A,D"); break;
    {"AND A,E", 0, 4, NULL},// case 0xA3: printf("AND A,E"); break;
    {"AND A,H", 0, 4, NULL},// case 0xA4: printf("AND A,H"); break;
    {"AND A,L", 0, 4, NULL},// case 0xA5: printf("AND A,L"); break;
    {"AND A,(HL)", 0, 8, NULL},// case 0xA6: printf("AND A,(HL)"); break;
    {"AND A,A", 0, 4, NULL},// case 0xA7: printf("AND A,A"); break;
    {"XOR A,B", 0, 4, NULL},// case 0xA8: printf("XOR A,B"); break;
    {"XOR A,C", 0, 4, NULL},// case 0xA9: printf("XOR A,C"); break;
    {"XOR A,D", 0, 4, NULL},// case 0xAA: printf("XOR A,D"); break;
    {"XOR A,E", 0, 4, NULL},// case 0xAB: printf("XOR A,E"); break;
    {"XOR A,H", 0, 4, NULL},// case 0xAC: printf("XOR A,H"); break;
    {"XOR A,L", 0, 4, NULL},// case 0xAD: printf("XOR A,L"); break;
    {"XOR A,(HL)", 0, 8, NULL},// case 0xAE: printf("XOR A,(HL)"); break;
    {"XOR A,A", 0, 4, &XOR_A_A},// case 0xAF: printf("XOR A,A"); break;

    {"OR A,B", 0, 4, NULL},// case 0xB0: printf("OR A,B"); break;
    {"OR A,C", 0, 4, NULL},// case 0xB1: printf("OR A,C"); break;
    {"OR A,D", 0, 4, NULL},// case 0xB2: printf("OR A,D"); break;
    {"OR A,E", 0, 4, NULL},// case 0xB3: printf("OR A,E"); break;
    {"OR A,H", 0, 4, NULL},// case 0xB4: printf("OR A,H"); break;
    {"OR A,L", 0, 4, NULL},// case 0xB5: printf("OR A,L"); break;
    {"OR A,(HL)", 0, 8, NULL},// case 0xB6: printf("OR A,(HL)"); break;
    {"OR A,A", 0, 4, NULL},// case 0xB7: printf("OR A,A"); break;
    {"CP A,B", 0, 4, NULL},// case 0xB8: printf("CP A,B"); break;
    {"CP A,C", 0, 4, NULL},// case 0xB9: printf("CP A,C"); break;
    {"CP A,D", 0, 4, NULL},// case 0xBA: printf("CP A,D"); break;
    {"CP A,E", 0, 4, NULL},// case 0xBB: printf("CP A,E"); break;
    {"CP A,H", 0, 4, NULL},// case 0xBC: printf("CP A,H"); break;
    {"CP A,L", 0, 4, NULL},// case 0xBD: printf("CP A,L"); break;
    {"CP A,(HL)", 0, 8, NULL},// case 0xBE: printf("CP A,(HL)"); break;
    {"CP A,A", 0, 4, NULL},// case 0xBF: printf("CP A,A"); break;

    {"RET NZ", 0, 8, NULL},// case 0xC0: printf("RET NZ"); break;
    {"POP BC", 0, 12, NULL},// case 0xC1: printf("POP BC"); break;
    {"JP NZ,$%02X%02X", 2, 12, NULL},// case 0xC2: printf("JP NZ,%02X%02X", code[2],code[1]); opbytes = 3; break;
    {"JP $%02X%02X", 2, 16, &JP_NN},// case 0xC3: printf("JP %02X%02X", code[2], code[1]); opbytes = 3; break;
    {"CALL NZ,$%02X%02X", 2, 12, NULL},// case 0xC4: printf("CALL NZ,%02X%02X", code[2], code[1]); opbytes = 3; break;
    {"PUSH BC", 0, 16, NULL},// case 0xC5: printf("PUSH BC"); break;
    {"ADD A,$%02X", 1, 8, NULL},// case 0xC6: printf("ADD A,%02X", code[1]); opbytes = 2; break;
    {"RST 00h", 0, 16, NULL},// case 0xC7: printf("RST 00h"); break;
    {"RET Z", 0, 8, NULL},// case 0xC8: printf("RET Z"); break;
    {"RET", 0, 16, NULL},// case 0xC9: printf("RET"); break;
    {"JP Z,$%02X%02X", 2, 12, NULL},// case 0xCA: printf("JP Z,%02X%02X", code[2], code[1]); opbytes = 3; break;
    {"PREFIX", 0, 4, NULL},// case 0xCB: 
    //     switch (*(code + 1))
    //     {
    //         default: printf("PREFIX"); opbytes = 3; break;
    //     }
    //     break;
    {"CALL Z,$%02X%02X", 2, 12, NULL},// case 0xCC: printf("CALL Z,%02X%02X", code[2], code[1]); opbytes = 3; break;
    {"CALL $%02X%02X", 2, 24, NULL},// case 0xCD: printf("CALL %02X%02X", code[2], code[1]); opbytes = 3; break;
    {"ADC A,$%02X", 1, 8, NULL},// case 0xCE: printf("ADC A,%02X", code[1]); opbytes = 2; break;
    {"RST 00h", 0, 16, NULL},// case 0xCF: printf("RST 00h"); break;
    
    {"RET NC", 0, 8, NULL},// case 0xD0: printf("RET NC"); break;
    {"POP DE", 0, 12, NULL},// case 0xD1: printf("POP DE"); break;
    {"JP NC,$%02X%02X", 2, 12, NULL},// case 0xD2: printf("JP NC,%02X%02X", code[2], code[1]); opbytes = 3; break;
    {"ERROR", 0, 0, NULL},
    {"CALL NC,$%02X%02X", 2, 12, NULL},// case 0xD4: printf("CALL NC,%02X%02X", code[2], code[1]); opbytes = 3; break; 
    {"PUSH DE", 0, 16, NULL},// case 0xD5: printf("PUSH DE"); break;
    {"SUB A,$%02X", 1, 8, NULL},// case 0xD6: printf("SUB A,%02X", code[1]); opbytes = 2; break;
    {"RST 10h", 0, 16, NULL},// case 0xD7: printf("RST 10h"); break;
    {"RET C", 0, 8, NULL},// case 0xD8: printf("RET C"); break;
    {"RETI", 0, 16, NULL},// case 0xD9: printf("RETI"); break;
    {"JP C,$%02X%02X", 2, 12, NULL},// case 0xDA: printf("JP C,%02X%02X", code[2], code[1]); opbytes = 3; break;
    {"ERROR", 0, 0, NULL},
    {"CALL C,$%02X%02X", 2, 12, NULL},// case 0xDC: printf("CALL C,%02X%02X", code[2], code[1]); opbytes = 3; break;
    {"ERROR", 0, 0, NULL},
    {"SBC A,$%02X", 1, 8, NULL},// case 0xDE: printf("SBC A,%02X", code[1]); opbytes = 2; break;
    {"RST 18h", 0, 16, NULL},// case 0xDF: printf("RST 18h"); break;

    {"LD ($FF00+$%02X),A", 1, 12, &LD_FF00_A},// case 0xE0: printf("LD ($FF00+%02X),A", code[1]); opbytes = 2; break;
    {"POP HL", 0, 12, NULL},// case 0xE1: printf("POP HL"); break;
    {"LD ($FF00+$C),A", 0, 8, NULL},// case 0xE2: printf("LD ($FF00+C),A"); break;
    {"ERROR", 0, 0, NULL},
    {"ERROR", 0, 0, NULL},
    {"PUSH HL", 0, 16, NULL},// case 0xE5: printf("PUSH HL"); break;
    {"AND A,$%02X", 1, 8, NULL},// case 0xE6: printf("AND A,%02X", code[1]); opbytes = 2; break;
    {"RST 20h", 0, 16, NULL},// case 0xE7: printf("RST 20h"); break;
    {"ADD SP,$%02X", 1, 16, NULL},// case 0xE8: printf("ADD SP,%02X", code[1]); break;
    {"JP HL", 0, 4, NULL},// case 0xE9: printf("JP HL"); break;
    {"LD ($%02X%02X),A", 2, 16, NULL},// case 0xEA: printf("LD (%02X%02X),A", code[2], code[1]); opbytes = 3; break;
    {"ERROR", 0, 0, NULL},
    {"ERROR", 0, 0, NULL},
    {"ERROR", 0, 0, NULL},
    {"XOR A,$%02X", 1, 8, NULL},// case 0xEE: printf("XOR A,%02X", code[1]); opbytes = 2; break;
    {"RST 28h", 0, 16, NULL},// case 0xEF: printf("RST 28h"); break;

    {"LD A,($FF00+$%02X)", 1, 12, NULL},// case 0xF0: printf("LD A,(FF00+%02X)", code[1]); opbytes = 2; break;
    {"POP AF", 0, 12, NULL},// case 0xF1: printf("POP AF"); break;
    {"LD A,($FF00+$C)", 0, 8, NULL},// case 0xF2: printf("LD A,(FF00+C)"); break;
    {"DI", 0, 4, NULL},// case 0xF3: printf("DI"); break;
    {"ERROR", 0, 0, NULL},
    {"PUSH AF", 0, 16, NULL},// case 0xF5: printf("PUSH AL"); break;
    {"OP A,$%02X", 1, 8, NULL},// case 0xF6: printf("OP A,%02X", code[1]); opbytes = 2; break;
    {"RST 30h", 0, 16, NULL},// case 0xF7: printf("RST 30h"); break;
    {"LD HL,SP+$%02X", 1, 12, NULL},// case 0xF8: printf("LD HL,SP+%02X", code[1]); break;
    {"LD SP,HL", 0, 8, NULL},// case 0xF9: printf("LD SP,HL"); break;
    {"LD A,($%02X%02X)", 2, 16, NULL},// case 0xFA: printf("LD A,(%02X%02X)", code[2], code[1]); break;
    {"EI", 0, 4, NULL},// case 0xFB: printf("EI"); break;
    {"ERROR", 0, 0, NULL},
    {"ERROR", 0, 0, NULL},
    {"CP A,$%02X", 1, 8, NULL},// case 0xFE: printf("CP A,%02X", code[1]); opbytes = 2; break;
    {"RST 38h", 0, 16, NULL},// case 0xFF: printf("RST 38h"); break;
};

const Instruction prefixInstructions[256] = {
    {"RLC B", 0, 8, NULL},
    {"RLC C", 0, 8, NULL},
    {"RLC D", 0, 8, NULL},
    {"RLC E", 0, 8, NULL},
    {"RLC H", 0, 8, NULL},
    {"RLC L", 0, 8, NULL},
    {"RLC (HL)", 0, 8, NULL},
    {"RLC A", 0, 8, NULL},

    {"RRC B", 0, 8, NULL},
    {"RRC C", 0, 8, NULL},
    {"RRC D", 0, 8, NULL},
    {"RRC E", 0, 8, NULL},
    {"RRC H", 0, 8, NULL},
    {"RRC L", 0, 8, NULL},
    {"RRC (HL)", 0, 8, NULL},
    {"RRC A", 0, 8, NULL},

    {"RL B", 0, 8, NULL},
    {"RL C", 0, 8, NULL},
    {"RL D", 0, 8, NULL},
    {"RL E", 0, 8, NULL},
    {"RL H", 0, 8, NULL},
    {"RL L", 0, 8, NULL},
    {"RL (HL)", 0, 8, NULL},
    {"RL A", 0, 8, NULL},

    {"RR B", 0, 8, NULL},
    {"RR C", 0, 8, NULL},
    {"RR D", 0, 8, NULL},
    {"RR E", 0, 8, NULL},
    {"RR H", 0, 8, NULL},
    {"RR L", 0, 8, NULL},
    {"RR (HL)", 0, 8, NULL},
    {"RR A", 0, 8, NULL},

    {"SLA B", 0, 8, NULL},
    {"SLA C", 0, 8, NULL},
    {"SLA D", 0, 8, NULL},
    {"SLA E", 0, 8, NULL},
    {"SLA H", 0, 8, NULL},
    {"SLA L", 0, 8, NULL},
    {"SLA (HL)", 0, 8, NULL},
    {"SLA A", 0, 8, NULL},

    {"SRA B", 0, 8, NULL},
    {"SRA C", 0, 8, NULL},
    {"SRA D", 0, 8, NULL},
    {"SRA E", 0, 8, NULL},
    {"SRA H", 0, 8, NULL},
    {"SRA L", 0, 8, NULL},
    {"SRA (HL)", 0, 8, NULL},
    {"SRA A", 0, 8, NULL},

    {"SWAP B", 0, 8, NULL},
    {"SWAP C", 0, 8, NULL},
    {"SWAP D", 0, 8, NULL},
    {"SWAP E", 0, 8, NULL},
    {"SWAP H", 0, 8, NULL},
    {"SWAP L", 0, 8, NULL},
    {"SWAP (HL)", 0, 8, NULL},
    {"SWAP A", 0, 8, NULL},

    {"SRL B", 0, 8, NULL},
    {"SRL C", 0, 8, NULL},
    {"SRL D", 0, 8, NULL},
    {"SRL E", 0, 8, NULL},
    {"SRL H", 0, 8, NULL},
    {"SRL L", 0, 8, NULL},
    {"SRL (HL)", 0, 8, NULL},
    {"SRL A", 0, 8, NULL},

    {"BIT 0,B", 0, 8, NULL},
    {"BIT 0,C", 0, 8, NULL},
    {"BIT 0,D", 0, 8, NULL},
    {"BIT 0,E", 0, 8, NULL},
    {"BIT 0,H", 0, 8, NULL},
    {"BIT 0,L", 0, 8, NULL},
    {"BIT 0,(HL)", 0, 8, NULL},
    {"BIT 0,A", 0, 8, NULL},

    {"BIT 1,B", 0, 8, NULL},
    {"BIT 1,C", 0, 8, NULL},
    {"BIT 1,D", 0, 8, NULL},
    {"BIT 1,E", 0, 8, NULL},
    {"BIT 1,H", 0, 8, NULL},
    {"BIT 1,L", 0, 8, NULL},
    {"BIT 1,(HL)", 0, 8, NULL},
    {"BIT 1,A", 0, 8, NULL},

    {"BIT 2,B", 0, 8, NULL},
    {"BIT 2,C", 0, 8, NULL},
    {"BIT 2,D", 0, 8, NULL},
    {"BIT 2,E", 0, 8, NULL},
    {"BIT 2,H", 0, 8, NULL},
    {"BIT 2,L", 0, 8, NULL},
    {"BIT 2,(HL)", 0, 8, NULL},
    {"BIT 2,A", 0, 8, NULL},

    {"BIT 3,B", 0, 8, NULL},
    {"BIT 3,C", 0, 8, NULL},
    {"BIT 3,D", 0, 8, NULL},
    {"BIT 3,E", 0, 8, NULL},
    {"BIT 3,H", 0, 8, NULL},
    {"BIT 3,L", 0, 8, NULL},
    {"BIT 3,(HL)", 0, 8, NULL},
    {"BIT 3,A", 0, 8, NULL},

    {"BIT 4,B", 0, 8, NULL},
    {"BIT 4,C", 0, 8, NULL},
    {"BIT 4,D", 0, 8, NULL},
    {"BIT 4,E", 0, 8, NULL},
    {"BIT 4,H", 0, 8, NULL},
    {"BIT 4,L", 0, 8, NULL},
    {"BIT 4,(HL)", 0, 8, NULL},
    {"BIT 4,A", 0, 8, NULL},

    {"BIT 5,B", 0, 8, NULL},
    {"BIT 5,C", 0, 8, NULL},
    {"BIT 5,D", 0, 8, NULL},
    {"BIT 5,E", 0, 8, NULL},
    {"BIT 5,H", 0, 8, NULL},
    {"BIT 5,L", 0, 8, NULL},
    {"BIT 5,(HL)", 0, 8, NULL},
    {"BIT 5,A", 0, 8, NULL},

    {"BIT 6,B", 0, 8, NULL},
    {"BIT 6,C", 0, 8, NULL},
    {"BIT 6,D", 0, 8, NULL},
    {"BIT 6,E", 0, 8, NULL},
    {"BIT 6,H", 0, 8, NULL},
    {"BIT 6,L", 0, 8, NULL},
    {"BIT 6,(HL)", 0, 8, NULL},
    {"BIT 6,A", 0, 8, NULL},

    {"BIT 7,B", 0, 8, NULL},
    {"BIT 7,C", 0, 8, NULL},
    {"BIT 7,D", 0, 8, NULL},
    {"BIT 7,E", 0, 8, NULL},
    {"BIT 7,H", 0, 8, NULL},
    {"BIT 7,L", 0, 8, NULL},
    {"BIT 7,(HL)", 0, 8, NULL},
    {"BIT 7,A", 0, 8, NULL},

    {"RES 0,B", 0, 8, NULL},
    {"RES 0,C", 0, 8, NULL},
    {"RES 0,D", 0, 8, NULL},
    {"RES 0,E", 0, 8, NULL},
    {"RES 0,H", 0, 8, NULL},
    {"RES 0,L", 0, 8, NULL},
    {"RES 0,(HL)", 0, 8, NULL},
    {"RES 0,A", 0, 8, NULL},

    {"RES 1,B", 0, 8, NULL},
    {"RES 1,C", 0, 8, NULL},
    {"RES 1,D", 0, 8, NULL},
    {"RES 1,E", 0, 8, NULL},
    {"RES 1,H", 0, 8, NULL},
    {"RES 1,L", 0, 8, NULL},
    {"RES 1,(HL)", 0, 8, NULL},
    {"RES 1,A", 0, 8, NULL},

    {"RES 2,B", 0, 8, NULL},
    {"RES 2,C", 0, 8, NULL},
    {"RES 2,D", 0, 8, NULL},
    {"RES 2,E", 0, 8, NULL},
    {"RES 2,H", 0, 8, NULL},
    {"RES 2,L", 0, 8, NULL},
    {"RES 2,(HL)", 0, 8, NULL},
    {"RES 2,A", 0, 8, NULL},

    {"RES 3,B", 0, 8, NULL},
    {"RES 3,C", 0, 8, NULL},
    {"RES 3,D", 0, 8, NULL},
    {"RES 3,E", 0, 8, NULL},
    {"RES 3,H", 0, 8, NULL},
    {"RES 3,L", 0, 8, NULL},
    {"RES 3,(HL)", 0, 8, NULL},
    {"RES 3,A", 0, 8, NULL},

    {"RES 4,B", 0, 8, NULL},
    {"RES 4,C", 0, 8, NULL},
    {"RES 4,D", 0, 8, NULL},
    {"RES 4,E", 0, 8, NULL},
    {"RES 4,H", 0, 8, NULL},
    {"RES 4,L", 0, 8, NULL},
    {"RES 4,(HL)", 0, 8, NULL},
    {"RES 4,A", 0, 8, NULL},

    {"RES 5,B", 0, 8, NULL},
    {"RES 5,C", 0, 8, NULL},
    {"RES 5,D", 0, 8, NULL},
    {"RES 5,E", 0, 8, NULL},
    {"RES 5,H", 0, 8, NULL},
    {"RES 5,L", 0, 8, NULL},
    {"RES 5,(HL)", 0, 8, NULL},
    {"RES 5,A", 0, 8, NULL},

    {"RES 6,B", 0, 8, NULL},
    {"RES 6,C", 0, 8, NULL},
    {"RES 6,D", 0, 8, NULL},
    {"RES 6,E", 0, 8, NULL},
    {"RES 6,H", 0, 8, NULL},
    {"RES 6,L", 0, 8, NULL},
    {"RES 6,(HL)", 0, 8, NULL},
    {"RES 6,A", 0, 8, NULL},

    {"RES 7,B", 0, 8, NULL},
    {"RES 7,C", 0, 8, NULL},
    {"RES 7,D", 0, 8, NULL},
    {"RES 7,E", 0, 8, NULL},
    {"RES 7,H", 0, 8, NULL},
    {"RES 7,L", 0, 8, NULL},
    {"RES 7,(HL)", 0, 8, NULL},
    {"RES 7,A", 0, 8, NULL},

    {"SET 0,B", 0, 8, NULL},
    {"SET 0,C", 0, 8, NULL},
    {"SET 0,D", 0, 8, NULL},
    {"SET 0,E", 0, 8, NULL},
    {"SET 0,H", 0, 8, NULL},
    {"SET 0,L", 0, 8, NULL},
    {"SET 0,(HL)", 0, 8, NULL},
    {"SET 0,A", 0, 8, NULL},

    {"SET 1,B", 0, 8, NULL},
    {"SET 1,C", 0, 8, NULL},
    {"SET 1,D", 0, 8, NULL},
    {"SET 1,E", 0, 8, NULL},
    {"SET 1,H", 0, 8, NULL},
    {"SET 1,L", 0, 8, NULL},
    {"SET 1,(HL)", 0, 8, NULL},
    {"SET 1,A", 0, 8, NULL},

    {"SET 2,B", 0, 8, NULL},
    {"SET 2,C", 0, 8, NULL},
    {"SET 2,D", 0, 8, NULL},
    {"SET 2,E", 0, 8, NULL},
    {"SET 2,H", 0, 8, NULL},
    {"SET 2,L", 0, 8, NULL},
    {"SET 2,(HL)", 0, 8, NULL},
    {"SET 2,A", 0, 8, NULL},

    {"SET 3,B", 0, 8, NULL},
    {"SET 3,C", 0, 8, NULL},
    {"SET 3,D", 0, 8, NULL},
    {"SET 3,E", 0, 8, NULL},
    {"SET 3,H", 0, 8, NULL},
    {"SET 3,L", 0, 8, NULL},
    {"SET 3,(HL)", 0, 8, NULL},
    {"SET 3,A", 0, 8, NULL},

    {"SET 4,B", 0, 8, NULL},
    {"SET 4,C", 0, 8, NULL},
    {"SET 4,D", 0, 8, NULL},
    {"SET 4,E", 0, 8, NULL},
    {"SET 4,H", 0, 8, NULL},
    {"SET 4,L", 0, 8, NULL},
    {"SET 4,(HL)", 0, 8, NULL},
    {"SET 4,A", 0, 8, NULL},

    {"SET 5,B", 0, 8, NULL},
    {"SET 5,C", 0, 8, NULL},
    {"SET 5,D", 0, 8, NULL},
    {"SET 5,E", 0, 8, NULL},
    {"SET 5,H", 0, 8, NULL},
    {"SET 5,L", 0, 8, NULL},
    {"SET 5,(HL)", 0, 8, NULL},
    {"SET 5,A", 0, 8, NULL},

    {"SET 6,B", 0, 8, NULL},
    {"SET 6,C", 0, 8, NULL},
    {"SET 6,D", 0, 8, NULL},
    {"SET 6,E", 0, 8, NULL},
    {"SET 6,H", 0, 8, NULL},
    {"SET 6,L", 0, 8, NULL},
    {"SET 6,(HL)", 0, 8, NULL},
    {"SET 6,A", 0, 8, NULL},

    {"SET 7,B", 0, 8, NULL},
    {"SET 7,C", 0, 8, NULL},
    {"SET 7,D", 0, 8, NULL},
    {"SET 7,E", 0, 8, NULL},
    {"SET 7,H", 0, 8, NULL},
    {"SET 7,L", 0, 8, NULL},
    {"SET 7,(HL)", 0, 8, NULL},
    {"SET 7,A", 0, 8, NULL},
};