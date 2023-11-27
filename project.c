#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch (ALUControl) {
        case 0:
            *ALUresult = A + B;
            break;
        case 1:
            *ALUresult = A - B;
            break;
        case 2:
            *ALUresult = A < B ? 1 : 0;
            break;
        case 3:
            *ALUresult = (unsigned)A < (unsigned)B ? 1 : 0;
            break;
        case 4:
            *ALUresult = A & B;  // Bitwise AND
            break;
        case 5:
            *ALUresult = A | B;  // Bitwise OR
            break;
        case 6:
            *ALUresult = B << 16;
            break;
        default:
            *ALUresult = ~A;  // Bitwise NOT
            break;
    }
    
    *Zero = (*ALUresult == 0) ? 1 : 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    *instruction = Mem[PC >> 2];
    return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    *op = (instruction >> 26) & 0x3F;     // instruction [31-26]
    *r1 = (instruction >> 21) & 0x1F;     // instruction [25-21]
    *r2 = (instruction >> 16) & 0x1F;     // instruction [20-16]
    *r3 = (instruction >> 11) & 0x1F;     // instruction [15-11]
    *funct = instruction & 0x3F;           // instruction [5-0]
    *offset = instruction & 0xFFFF;        // instruction [15-0]
    *jsec = instruction & 0x3FFFFFF;       // instruction [25-0]
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{

}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    *extended_value = offset;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

}