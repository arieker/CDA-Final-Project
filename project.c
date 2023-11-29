#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch (ALUControl) {
        case 0x0: *ALUresult = A + B; break;
        case 0x1: *ALUresult = A - B; break;
        case 0x2: *ALUresult = (unsigned)A < (unsigned)B ? 1 : 0; break;
        case 0x3: *ALUresult = A < B ? 1 : 0; break;
        case 0x4: *ALUresult = A & B; break;
        case 0x5: *ALUresult = A | B; break;
        case 0x6: *ALUresult = B << 16; break;
        case 0x7: *ALUresult = ~A; break;
    }
    
    *Zero = (*ALUresult == 0) ? 1 : 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // Halt condidition for non word-alligned address or over max memory usage
    if (PC % 4 != 0) // ? this MAY be wrong idk lol
    {
        return 1;
    }
    
    *instruction = Mem[PC >> 2];
    return 0;
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    *op = (instruction & 0x0000003f) >> 26;      // instruction [31-26]
    *r1 = (instruction & 0x0000001F) >> 21 ;     // instruction [25-21]
    *r2 = (instruction & 0x0000001F) >> 16;     // instruction [20-16]
    *r3 = (instruction & 0x0000001F) >> 11;     // instruction [15-11]
    *funct = instruction & 0x0000003F;           // instruction [5-0]
    *offset = instruction & 0X0000FFFF;        // instruction [15-0]
    *jsec = instruction & 0x03FFFFFF;       // instruction [25-0]
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // Set defaults
    controls -> RegDst = 0;
    controls -> Jump = 0;
    controls -> Branch = 0;
    controls -> MemRead = 0;
    controls -> MemtoReg = 0;
    controls -> ALUOp = 0;
    controls -> MemWrite = 0;
    controls -> ALUSrc = 0;
    controls -> RegWrite = 0;

    // Decode the instruction using the opcode (op)
    switch (op) {
        case 0x0:
            controls -> RegDst = 1;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 7;
            controls -> MemWrite = 0;
            controls -> ALUSrc = 0;
            controls -> RegWrite = 1;
            break;
        // addi, 0000 1000 -> 8.
        case 0x8:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 0;
            controls -> MemWrite = 0;
            controls -> ALUSrc = 1;
            controls -> RegWrite = 1;
            break;
        // lw
        case 0x23:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 1;
            controls -> MemtoReg = 1;
            controls -> ALUOp = 0;
            controls -> MemWrite = 0;
            controls -> ALUSrc = 1;
            controls -> RegWrite = 1;
            break;
        // sw
        case 0x2b:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 0;
            controls -> MemWrite = 1;
            controls -> ALUSrc = 1;
            controls -> RegWrite = 0;
            break;
        // lui
        case 0xf:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 6;
            controls -> MemWrite = 0;
            controls -> ALUSrc = 1;
            controls -> RegWrite = 1;
            break;
        // slti
        case 0xa:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 2;
            controls -> MemWrite = 0;
            controls -> ALUSrc = 1;
            controls -> RegWrite = 1;
            break;
        // sltiu
        case 0xb:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 3;
            controls -> MemWrite = 0;
            controls -> ALUSrc = 1;
            controls -> RegWrite = 1;
            break;
        // beq
        case 0x4:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 1;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 1;
            controls -> MemWrite = 0;
            controls -> ALUSrc = 0;
            controls -> RegWrite = 0;
            break;
        // jump
        case 0x2:
            controls -> RegDst = 0;
            controls -> Jump = 1;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 0;
            controls -> MemWrite = 0;
            controls -> ALUSrc = 0;
            controls -> RegWrite = 0;
            break;
        default:
            return 1;
    }
    return 0;
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
    if (offset >> 15 == 1)
    {
        *extended_value = offset | 0xffff0000;
    }
    else
    {
        *extended_value = offset;
    }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    if (ALUSrc == 0)
    {
        switch (funct)
        {
        case 0x21:
            ALUOp = 0;
            break;
        case 0x23:
            ALUOp = 1;
            break;
        case 0x24:
            ALUOp = 4;
            break;
        case 0x2a:
            ALUOp = 2;
            break;
        case 0x2b:
            ALUOp = 3;
            break;
        default:
            return 1;
        }

        ALU(data1, data2, ALUOp, ALUresult, Zero);
    }
    else if (ALUSrc == 1) {
        
        ALU(data1, extended_value, ALUOp, ALUresult, Zero);
    }
    
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    if (ALUresult % 4 != 0 && (MemWrite == 1 || MemRead == 1))
    {
        return 1;
    }
    if (MemWrite == 1)
    {
        Mem[ALUresult >> 2] = data2;
    }
    if (MemRead == 1)
    {
        *memdata = Mem[ALUresult >> 2];
    }
    
    return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
   if (RegWrite == 1)
   {
        if (MemtoReg == 1)
        {
            if (RegDst == 1)
            {
                Reg[r3] = memdata;
            }
            else
            {
                Reg[r2] = memdata;
            }
        }
        else
        {
            if (RegDst == 1)
            {
                Reg [r3] = ALUresult;
            }
            else
            {
                Reg [r2] = ALUresult;
            }
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    *PC += 4;
    if (Jump == 1)
    {
        *PC = (jsec << 2) | (*PC | 0xf0000000);
    }
    else if (Branch == 1 && Zero)
    {
        *PC += (extended_value << 2);
    }
}