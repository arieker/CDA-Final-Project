#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch (ALUControl) {
        case 0: *ALUresult = A + B; break;
        case 1: *ALUresult = A - B; break;
        case 2: *ALUresult = (int)A < (int)B ? 1 : 0; break;
        case 3: *ALUresult = A < B ? 1 : 0; break;
        case 4: *ALUresult = A & B; break;
        case 5: *ALUresult = A | B; break;
        case 6: *ALUresult = B << 16; break;
        case 7: *ALUresult = ~A; break;
    }
    
    *Zero = (*ALUresult == 0) ? 1 : 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // Halt condidition for non word-alligned address or over max memory usage
    if (PC % 4 != 0 || PC > 65535)
        return 1;

    *instruction = Mem[PC >> 2];

    // Halt condition for illegal instruction 
    if (*instruction == 0)
        return 1;

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
    // ! This is untested, I removed defaults because I don't think we need to set them?
    // ! but we may need to, pending deletion -anthony

    // Decode the instruction using the opcode (op)
    switch (op) {
        case 0x00:
            controls->RegDst = 1;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 7;
            controls->MemWrite = 0;
            controls->ALUSrc = 0;
            controls->RegWrite = 1;
            break;
        case 0x0d:
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 5;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;
        case 0x23:
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 1;
            controls->MemtoReg = 1;
            controls->ALUOp = 0;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;
        case 0x2b:
            controls->RegDst = 2;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 2;
            controls->ALUOp = 0;
            controls->MemWrite = 1;
            controls->ALUSrc = 1;
            controls->RegWrite = 0;
            break;
        case 0x0a:
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 2;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;
        case 0x04:
            controls->RegDst = 2;
            controls->Jump = 0;
            controls->Branch = 1;
            controls->MemRead = 0;
            controls->MemtoReg = 2;
            controls->ALUOp = 0;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 0;
            break;
        case 0x01:
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 1;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 6;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 0;
            break;
        case 0x02:
            controls->RegDst = 2;
            controls->Jump = 1;
            controls->Branch = 2;
            controls->MemRead = 2;
            controls->MemtoReg = 2;
            controls->ALUOp = 0;
            controls->MemWrite = 0;
            controls->ALUSrc = 2;
            controls->RegWrite = 0;
            break;
        default:
            return 1;
    }
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    // Do halt conditions need to be accounted for here?

    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    *extended_value = (unsigned)((int)(short)(offset & 0xFFFF));
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
    if (ALUresult % 4 != 0 || ALUresult > 65535)
    {
        return 1;
    }

    if (MemWrite == 0 && MemRead == 0)
    {
        // Neither are happening so maybe halt?
        return 1;
    }
    else if (MemWrite == 1)
    {
        Mem[ALUresult] = data2;
    }
    else // Read
    {
        *memdata = Mem[ALUresult];
    }
    
    return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    // Instructions: Write the data (ALUresult or memdata) to a register (Reg) addressed by r2 or r3.
    unsigned write_data;

    // Determine the data to be written to the register
    if (MemtoReg) {
        // Write data from memory (memdata)
        write_data = memdata;
    } else {
        // Write data from ALU (ALUresult)
        write_data = ALUresult;
    }

    // Determine the destination register address
    unsigned reg_address;
    if (RegDst) {
        // Use r3 as the destination register address
        reg_address = r3;
    } else {
        // Use r2 as the destination register address
        reg_address = r2;
    }

    // Write data to register if RegWrite is asserted
    if (RegWrite) {
        Reg[reg_address] = write_data;
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    if (Branch && Zero) { // Branch if Zero, characters when 1 can be used as bools :)
        *PC += extended_value;
    } else if (Jump) { // Jump to jsec
        *PC = jsec;
    } else {
        *PC += 4;
    }
}