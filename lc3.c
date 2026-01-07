#include <stdio.h>
#include <stdint.h>
#include <signal.h>

#define MEMMORY_CAPACITY 1<<16
uint16_t mem[MEMMORY_CAPACITY];

uint16_t sign_extend(uint16_t x, int bitcount)
{
    if ((x >> (bitcount - 1)) & 1)
    {
        uint16_t mask = 0xFFF << bitcount;
        x |= mask;
    }
    return x;
}

void update_flags(uint16_t r)
{
    if (reg[r]==0)
    {
        reg[R_COND] = FL_ZRO;
    }
    else if (reg[r]>>15)
    {
        reg[R_COND] = FL_POS;
    }
    else
    {
        reg[R_COND] = FL_NEG;
    }
}

enum
{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,
    R_COND,
    R_COUNT
};

uint16_t reg[R_COUNT];

enum
{
    OP_ADD = 0x1,
    OP_AND = 0x5,
    OP_BR = 0x0,
    OP_JMP = 0xC,
    OP_JSR = 0x4,
    OP_LD = 0x2,
    OP_LDI = 0xA,
    OP_LDR = 0x6,
    OP_LEA = 0xE,
    OP_NOT = 0x9,
    OP_RTI  = 0x8,
    OP_ST = 0x3,
    OP_STI = 0xB,
    OP_STR = 0x7,
    OP_TRAP = 0xF,
    OP_RES = 0xD
};

enum
{
    FL_NEG = 1<<0,
    FL_ZRO = 1<<1,
    FL_POS = 1<<2
};


void mem_write()
{

}

uint16_t mem_read()
{

}



int main(int argc, const char* argv[])
{
    /*Load Arguments*/
    if (argc<2)
    {
        printf("lc-3 [image-file]......\n");
        exit(2);
    }
    
    for (int j = 1; j < argc; ++j)
    {
        if (!read_image(argv[j]))
        {
            printf("Failed to load image file: %s\n", argv[j]);
            exit(1);
        } 
    }
    

    /*SETUP*/


    /*Condition flag should be given at any point. Initialize to Z*/
    reg[R_COND] = FL_ZRO;

    /*Memory location where program starts*/
    enum {PC_START = 0x3000};
    reg[R_PC] = PC_START;

    int running = 1;
    while (running)
    {
        /*FETCH*/
        uint16_t inst = mem_read(reg[R_PC]++);
        uint16_t op = inst >> 12;

        switch (op)
        {
        case OP_ADD:
            {
                /*opcode fields*/
                uint16_t r0 = (inst >> 9) & 0x7;
                uint16_t r1 = (inst >> 6) & 0x7;
                uint16_t imm_flag = (inst >> 5) & 0x1;

                if (imm_flag)
                {
                    uint16_t imm = sign_extend(inst & 0x1F, 5);
                    reg[r0] = reg[r1] + imm;
                }
                else
                {
                    uint16_t r2 = inst & 0x7;
                    reg[r0] = reg[r1] + reg[r2];
                }
                update_flags(r0);
            }
            break;

        case OP_AND:
            {
                uint16_t r0 = (inst >> 9) & 0x7;
                uint16_t r1 = (inst >> 6) & 0x7;
                uint16_t imm_flag = (inst >> 5) & 0x1;

                if (imm_flag)
                {
                    uint16_t imm5 = sign_extend(inst & 0x1F, 5);
                    reg[r0] = reg[r1] & imm5;
                }
                else
                {
                    uint16_t r2 = inst & 0x7;
                    reg[r0] = reg[r1] & reg[r2];
                }
                update_flags(r0);
            }
            break;

        case OP_BR:
            {
                uint16_t n = (inst >> 11) & 0x1;
                uint16_t z = (inst >> 10) & 0x1;
                uint16_t p = (inst >> 9) & 0x1;
                uint16_t PCoffset9 = inst & 0x1FF;

                if (n| z| p)
                {
                    reg[R_PC] = reg[R_PC] + sign_extend(PCoffset9, 9);
                }
                update_flags(R_PC);
            }
            break;

        case OP_JMP:
            {
                uint16_t base_r = (inst >> 6) & 0x7;
                reg[R_PC] = reg[base_r];
                update_flags(R_PC);
            }
            break;

        case OP_JSR:
            {
                uint16_t r_flag = (inst >> 11) & 0x1;
                if (r_flag)
                {
                    uint16_t base_r = (inst >> 6) & 0x7;
                    reg[R_PC] = reg[base_r];
                }
                else
                {
                    uint16_t PCoffset11 = inst & 0x7FF;
                    reg[R_PC] = reg[R_PC] + sign_extend(PCoffset11, 11);
                }
                update_flags(R_PC);
            }
            break;

        case OP_LD:
            {
                uint16_t PCoffset9 = inst & 0x1FF;
                uint16_t r0 = (inst >> 9) & 0x7;
                reg[r0] = mem[reg[R_PC] + sign_extend(PCoffset9, 9)];
                update_flags(r0);
            }
            break;

        case OP_LDI:
            {
                uint16_t PCoffset9 = inst & 0x1FF;
                uint16_t r0 = (inst >> 9) & 0x7;
                reg[r0] = mem[mem[reg[R_PC] + sign_extend(PCoffset9, 9)]];
                update_flags(r0);
            }
            break;

        case OP_LDR:
            {
                uint16_t offset6 = inst & 0x3F;
                uint16_t base_r = (inst >> 6) & 0x7;
                uint16_t r0 = (inst >> 9) & 0x7;
                reg[r0] = mem[reg[base_r] + sign_extend(offset6,6)];
                update_flags(r0);
            }
            break;

        case OP_LEA:
            {
                uint16_t PCoffset9 = inst & 0x1FF;
                uint16_t r0 = (inst >> 9) & 0x7;
                reg[r0] = reg[R_PC] + sign_extend(PCoffset9, 9);
                update_flags(r0);
            }
            break;

        case OP_NOT:
            {
                uint16_t r0 = (inst >> 9) & 0x7;
                uint16_t r1 = (inst >> 6) & 0x7;
                update_flags(r0);
            }
            break;

        case OP_ST:
            {
                uint16_t PCoffset9 = inst & 0x1FF;
                uint16_t r0 = (inst >> 9) & 0x7;
                mem[reg[R_PC] + sign_extend(PCoffset9, 9)] = reg[r0];
            }
            break;

        case OP_STI:
            {
                uint16_t PCoffset9 = inst & 0x1FF;
                uint16_t r0 = (inst >> 9) & 0x7;
                mem[mem[reg[R_PC] + sign_extend(PCoffset9, 9)]] = reg[r0];
            }
            break;

        case OP_STR:
            {
                uint16_t offset6 = inst & 0x3F;
                uint16_t base_r = (inst >> 6) & 0x7;
                uint16_t r0 = (inst >> 9) & 0x7;
                mem[reg[base_r] + sign_extend(offset6,6)] = reg[r0];
            }
            break;

        case OP_TRAP:
            {
                uint16_t trapvect8 = inst & 0xFF;
                reg[R_R7] = reg[R_PC];
                reg[R_PC] = mem[trapvect8];
            }

        case OP_RTI:
        case OP_RES:
        default:
            abort();
            break;
        }
    }
    
}