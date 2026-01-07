#include <stdio.h>
#include <stdint.h>
#include <signal.h>

#define MEMMORY_CAPACITY 1<<16
uint16_t mem[MEMMORY_CAPACITY];

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
    OP_ADD,
    OP_AND,
    OP_BR,
    OP_JMP,
    OP_JSR,
    OP_JSRR,
    OP_LD,
    OP_LDR,
    OP_LEA,
    OP_NOT,
    OP_RET,
    OP_RTI,
    OP_ST,
    OP_STI,
    OP_STR,
    OP_TRAP
};

enum
{
    FL_NEG = 1<<0,
    FL_ZRO = 1<<1,
    FL_POS = 1<<2
};

int main(int argc, const char* argv[])
{

}