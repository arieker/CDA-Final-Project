#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch (ALUControl) {
        case 0: *ALUresult = A + B; break;
        case 1: *ALUresult = A - B; break;
        case 2: *ALUresult = A < B ? 1 : 0; break;
        case 3: *ALUresult = (unsigned)A < (unsigned)B ? 1 : 0; break;
        case 4: *ALUresult = A & B; break;
        case 5: *ALUresult = A | B; break;
        case 6: *ALUresult = B << 16; break;
        default: *ALUresult = ~A; break;
    }
    
    *Zero = (*ALUresult == 0) ? 1 : 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // Halt condidition for non word-alligned address
    if (PC % 4 != 0)
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
    // ! Halt conditions need to be set for this function, it also may not work since it's not accounting for all possible instructions
    // ! AKA this needs work I'm not confident in it, -anthony

    // Set to default values to avoid potential headache
    controls->RegDst = 2;    // don't care
    controls->Jump = 2;      // don't care
    controls->Branch = 2;    // don't care
    controls->MemRead = 0;   // disabled
    controls->MemtoReg = 2;  // don't care
    controls->ALUOp = 0;     // default value
    controls->MemWrite = 0;  // disabled
    controls->ALUSrc = 0;    // default value
    controls->RegWrite = 0;  // disabled

    // Decode the instruction using the opcode (op)
    switch (op) {
        case 0b000000: // R-type instruction
            controls->RegDst = 1;
            controls->ALUOp = 0b111;
            controls->RegWrite = 1;
            break;
        case 0b100011: // lw instruction
            controls->MemRead = 1;
            controls->MemtoReg = 1;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;
        case 0b101011: // sw instruction
            controls->MemWrite = 1;
            controls->ALUSrc = 1;
            break;
        case 0b000100: // beq instruction
            controls->Branch = 1;
            controls->ALUOp = 0b010;
            break;
        case 0b000010: // j instruction
            controls->Jump = 1;
            break;
        case 0b000101: // bne instruction
            controls->Branch = 1;
            controls->ALUOp = 0b011;
            break;
        case 0b001000: // addi instruction
            controls->ALUOp = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            break;
        //case 0xFFFFFFFF: // ! my attempt at halt condition (replace with the actual value)
            //return 1; COMMENTED OUT FOR NOW, I DONT KNOW HOW TO DO THE HALT CASE YET :)
        default:
            // Handle unrecognized opcode
            break;
    }

    return 0;
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
    /* temp commented out for error
    unsigned operand1 = NULL; tempcommented out
    switch(ALUSrc){
        case 1: operand1 = extended_value; break;
        case 2: operand1 = data1; break;
        default: operand1 = data2; break;
    }
    

    
    ALU(data1, data2, operand1, ALUresult, Zero);

    return 0;
    */ 
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    if (ALUresult % 4 != 0)
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

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    // Assume a simple scenario where you update PC based on Branch, Jump, and Zero conditions.
    if (Branch && Zero) { // Branch if Zero, characters when 1 can be used as bools :)
        *PC += extended_value;
    } else if (Jump) { // Jump to jsec
        *PC = jsec;
    } else {
        *PC += 4;
    }
}