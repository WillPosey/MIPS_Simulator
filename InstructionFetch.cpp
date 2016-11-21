/************************************************
 * 		InstructionFetch.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#include "InstructionFetch.h"

#include <iostream>
#include <sstream>

using namespace std;

/**************************************************************
 *
 * 		InstructionFetch Constructor
 *
 **************************************************************/
InstructionFetch::InstructionFetch(MainMemory& memRef, InstructionQueue& instrQRef, BranchTargetBuffer& btbRef)
:   memory(memRef),
    instrQueue(instrQRef),
    btb(btbRef)
{
    UpdateProgramCounter(ADDRESS_START);
    isBranch = false;
}

/**************************************************************
 *
 * 		InstructionFetch::RunCycle
 *
 **************************************************************/
void InstructionFetch::RunCycle()
{
    GetNextInstruction();
    if(IsBranchInstruction())
    {
        if(btb.Hit(programCounter))
        {
            if(btb.GetPrediction(programCounter))
                UpdateProgramCounter(btb.GetDestinationAddress(programCounter));
            else
                IncrementProgramCounter();
        }
        else
        {
            btb.CreateEntry(programCounter, branchDestination, 0);
            IncrementProgramCounter();
        }
    }
    else
        IncrementProgramCounter();
    WriteToQueue();
}

/**************************************************************
 *
 * 		InstructionFetch::UpdateProgramCounter
 *
 **************************************************************/
void InstructionFetch::UpdateProgramCounter(int PC)
{
    programCounter = PC;
}

/**************************************************************
 *
 * 		InstructionFetch::IncrementProgramCounter
 *
 **************************************************************/
void InstructionFetch::IncrementProgramCounter()
{
    programCounter += 4;
}

/**************************************************************
 *
 * 		InstructionFetch::GetNextInstruction
 *
 **************************************************************/
void InstructionFetch::GetNextInstruction()
{
    instructionBinary = memory[programCounter];
    GetInstructionString();
}

/**************************************************************
 *
 * 		InstructionFetch::WriteToQueue
 *
 **************************************************************/
void InstructionFetch::WriteToQueue()
{
    instrQueue.Write(instructionString);
}

/**************************************************************
 *
 * 		InstructionFetch::IsBranchInstruction
 *
 **************************************************************/
bool InstructionFetch::IsBranchInstruction()
{
    bool branch = isBranch;
    isBranch = false;
    return branch;
}

/**************************************************************
 *
 * 		InstructionFetch::GetInstructionString
 *
 **************************************************************/
void InstructionFetch::GetInstructionString()
{
    GetBinaryString();
    BinaryStringToInstruction();
}

/**************************************************************
 *
 * 		InstructionFetch::GetBinaryString
 *
 **************************************************************/
void InstructionFetch::GetBinaryString()
{
    char bitVal;
    char instrString[33];
    int index = 0;
    for(int bit=31; bit>=0; bit--)
    {
        bitVal = ( instructionBinary & (1<<bit) ) ? '1' : '0';
        instrString[index] = bitVal;
        index++;
    }
    instrString[32] = '\0';
    instructionBinaryString = string(instrString);
}

/**************************************************************
 *
 * 		InstructionFetch::BinaryStringToInstruction
 *
 **************************************************************/
void InstructionFetch::BinaryStringToInstruction()
{
    string opcode, name;
    InstructionType type;

    opcode = GetOpcode(instructionBinaryString);
    type = GetInstructionType(opcode);
    name = GetInstructionName(type, opcode, instructionBinaryString, instructionBinary);
    instructionString = name;

    switch(type)
    {
        case MEMORY:
            instructionString += " " + GetRegister((instructionBinary & MEMORY_RT_MASK)>>MEMORY_RT_SHIFT) + ", "
                                    + GetMemoryOffset(instructionBinary & MEMORY_OFFSET_MASK) + "("
                                    + GetRegister((instructionBinary & MEMORY_BASE_MASK)>>MEMORY_BASE_SHIFT) + ")";
            break;
        case IMMEDIATE:
            instructionString += " " + GetRegister((instructionBinary & IMMEDIATE_RT_MASK)>>IMMEDIATE_RT_SHIFT) + ", "
                                    + GetRegister((instructionBinary & IMMEDIATE_RS_MASK)>>IMMEDIATE_RS_SHIFT) + ", ";
            if(!name.compare("ADDIU"))
                instructionString += GetImmediateValue((instructionBinary & IMMEDIATE_VALUE_MASK),true);
            else
                instructionString += GetImmediateValue(instructionBinary & IMMEDIATE_VALUE_MASK);
            break;
        case BRANCH:
            isBranch = true;
            instructionString += " " + GetRegister((instructionBinary & BRANCH_RS_MASK)>>BRANCH_RS_SHIFT) + ", ";
            if( !name.compare("BEQ") || !name.compare("BNE") )
                instructionString += GetRegister((instructionBinary & BRANCH_RT_MASK)>>BRANCH_RT_SHIFT) + ", ";
            instructionString += "#" + GetBranchOffset(instructionBinary & BRANCH_OFFSET_MASK);
            break;
        case JUMP:
            instructionString += " " + GetJumpAddress( (instructionBinary & JUMP_TARGET_MASK), programCounter);
            break;
        case REGIMM:
            instructionString += " " + GetRegister((instructionBinary & REGIMM_RS_MASK)>>REGIMM_RS_SHIFT) + ", #"
                                    + GetBranchOffset(instructionBinary & REGIMM_OFFSET_MASK);
            break;
        case SPECIAL:
            if( !name.compare("BREAK") || !name.compare("NOP") )
                break;
            instructionString += " " + GetRegister((instructionBinary & SPECIAL_RD_MASK)>>SPECIAL_RD_SHIFT) + ", ";
            if( name.compare("SLL") && name.compare("SRL") && name.compare("SRA") )
                instructionString += GetRegister((instructionBinary & SPECIAL_RS_MASK)>>SPECIAL_RS_SHIFT) + ", ";
            instructionString += GetRegister((instructionBinary & SPECIAL_RT_MASK)>>SPECIAL_RT_SHIFT);
            if( !name.compare("SLL") || !name.compare("SRL") || !name.compare("SRA") )
                instructionString += ", #" + GetShiftAmount((instructionBinary & SPECIAL_SA_MASK)>>SPECIAL_SA_SHIFT);
            break;
        default:
            instructionString = "";
            break;
    }
}

/**********************************************************************************************
 * 		Method:			InstructionFetch::GetInstructionType
 *
 * 		Parameters:     string: the opcode of the binary instruction
 * 		Return:         InstructionType: the type of the instruction
 * 		Description:    used the opcode to determine the type of the corresponding instruction
 **********************************************************************************************/
InstructionType InstructionFetch::GetInstructionType(string opcode)
{
    if(opcode[0] == '1')
        return MEMORY;
    if(opcode[2] == '1')
        return IMMEDIATE;
    if(opcode[3] == '1')
        return BRANCH;
    if(opcode[4] == '1')
        return JUMP;
    if(opcode[5] == '1')
        return REGIMM;
    return SPECIAL;
}

/****************************************************************************************************************************
 * 		Method:			InstructionFetch::GetInstructionName
 *
 * 		Parameters:     MemoryLocation: structure holding location information, used for instruction type
 * 		Return:         string: assembly name of instruction
 * 		Description:    uses the MemoryLocation type to return the name of the instruction at that address as a string
 ****************************************************************************************************************************/
string InstructionFetch::GetInstructionName(InstructionType type, string opcode, string binaryString, uint32_t binaryValue)
{
    string specialInstr;
    switch(type)
    {
        case MEMORY:
            return GetMemoryInstructionName(opcode);
        case IMMEDIATE:
            return GetImmediateInstructionName(opcode);
        case BRANCH:
            return GetBranchInstructionName(opcode);
        case JUMP:
            return GetJumpInstructionName();
        case REGIMM:
            return GetRegimmInstructionName(binaryString.substr(11,5));
        case SPECIAL:
            specialInstr = GetSpecialInstructionName(binaryString.substr(26,6));
            if(!specialInstr.compare("SLL"))
                if(binaryValue == 0)
                    specialInstr = "NOP";
            return specialInstr;
    }
    return " ";
}

/**************************************************************************************************
 * 		Method:			InstructionFetch::GetMemoryInstructionName
 *
 * 		Parameters:     string: the opcode
 * 		Return:         string: the instrucion name for a memory type instruction
 * 		Description:    returns the instruction name for a memory type instruction using the opcode
 **************************************************************************************************/
string InstructionFetch::GetMemoryInstructionName(string opcode)
{
    if(opcode[2] == '1')
        return instructionNames[MEMORY][SW];
    else
        return instructionNames[MEMORY][LW];
}

/**********************************************************************************************************
 * 		Method:			InstructionFetch::GetImmediateInstructionName
 *
 * 		Parameters:     string: the opcode
 * 		Return:         string: the instrucion name for an immediate type instruction
 * 		Description:    returns the instruction name for an immediate type instruction using the opcode
 **********************************************************************************************************/
string InstructionFetch::GetImmediateInstructionName(string opcode)
{
    if(opcode[4] == '1')
        return instructionNames[IMMEDIATE][SLTI];
    else if(opcode[5] == '1')
        return instructionNames[IMMEDIATE][ADDIU];
    else
        return instructionNames[IMMEDIATE][ADDI];
}

/**********************************************************************************************************
 * 		Method:			InstructionFetch::GetBranchInstructionName
 *
 * 		Parameters:     string: the opcode
 * 		Return:         string: the instrucion name for a branch type instruction
 * 		Description:    returns the instruction name for a branch type instruction using the opcode
 **********************************************************************************************************/
string InstructionFetch::GetBranchInstructionName(string opcode)
{
    if(opcode[4] == '1')
    {
        if(opcode[5] == '1')
            return instructionNames[BRANCH][BGTZ];
        else
            return instructionNames[BRANCH][BLEZ];
    }
    else if(opcode[5] == '1')
        return instructionNames[BRANCH][BNE];
    else
        return instructionNames[BRANCH][BEQ];
}

/******************************************************************************************
 * 		Method:			InstructionFetch::GetJumpInstructionName
 *
 * 		Parameters:
 * 		Return:         string: the instrucion name for a jump type instruction
 * 		Description:    returns the instruction name for a jump type instruction
 ******************************************************************************************/
string InstructionFetch::GetJumpInstructionName()
{
    return instructionNames[JUMP][J];
}

/************************************************************************************************************
 * 		Method:			InstructionFetch::GetRegimmInstructionName
 *
 * 		Parameters:     string: the function type for regimm instruction
 * 		Return:         string: the instrucion name for a regimm type instruction
 * 		Description:    returns the instruction name for a regimm type instruction using the function
 ************************************************************************************************************/
string InstructionFetch::GetRegimmInstructionName(string funct)
{
    if(funct[4] == '1')
        return instructionNames[REGIMM][BGEZ];
    else
        return instructionNames[REGIMM][BLTZ];
}

/************************************************************************************************************
 * 		Method:			InstructionFetch::GetSpecialInstructionName
 *
 * 		Parameters:     string: the function type for special instruction
 * 		Return:         string: the instrucion name for a special type instruction
 * 		Description:    returns the instruction name for a special type instruction using the function
 ************************************************************************************************************/
string InstructionFetch::GetSpecialInstructionName(string funct)
{
    if(funct[0] == '1')
    {
        if(funct[2] == '1')
        {
            if(funct[5] == '1')
                return instructionNames[SPECIAL][SLTU];
            else
                return instructionNames[SPECIAL][SLT];
        }
        else if(funct[3] == '1')
        {
            if(funct[4] == '1')
            {
                if(funct[5] == '1')
                    return instructionNames[SPECIAL][NOR];
                else
                    return instructionNames[SPECIAL][XOR];

            }
            else if(funct[5] == '1')
                return instructionNames[SPECIAL][OR];
            else
                return instructionNames[SPECIAL][AND];
        }
        else if(funct[4] == '1')
        {
            if(funct[5] == '1')
                return instructionNames[SPECIAL][SUBU];
            else
                return instructionNames[SPECIAL][SUB];
        }
        else
        {
            if(funct[5] == '1')
                return instructionNames[SPECIAL][ADDU];
            else
                return instructionNames[SPECIAL][ADD];
        }
    }
    else if(funct[2] == '1')
        return instructionNames[SPECIAL][BREAK];
    else if(funct[4] == '1')
    {
        if(funct[5] == '1')
            return instructionNames[SPECIAL][SRA];
        else
            return instructionNames[SPECIAL][SRL];
    }
    else
        return instructionNames[SPECIAL][SLL];
}

/******************************************************************************************
 * 		Method:			InstructionFetch::GetRegister
 *
 * 		Parameters:     uint8_t: 5 bit register value
 * 		Return:         string: register name
 * 		Description:    uses 5 bit register value to index global registerNames array
 ******************************************************************************************/
string InstructionFetch::GetRegister(uint8_t regVal)
{
    return registerNames[regVal];
}

/**********************************************************************************
 * 		Method:			InstructionFetch::GetJumpAddress
 *
 * 		Parameters:     uint32_t: 26 bit jump address
 *                      uint32_t: current program counter address
 * 		Return:         string: string representation of jump address
 * 		Description:    takes jump address and program counter address to return
 *                      string representation of resolved jump address
 **********************************************************************************/
string InstructionFetch::GetJumpAddress(uint32_t jumpAddress, uint32_t pcAddress)
{
    uint32_t address;
    stringstream oStr;
    oStr << "#";
    address = (jumpAddress<<2) | (pcAddress&JUMP_PC_MASK);
    oStr << address;
    return oStr.str();
}

/******************************************************************************************
 * 		Method:			InstructionFetch::GetShiftAmount
 *
 * 		Parameters:     uint8_t: 5 bit shift amount
 * 		Return:         string: decimal shift amount
 * 		Description:    converts 5 bit binary shift amount to decimal string
 ******************************************************************************************/
string InstructionFetch::GetShiftAmount(uint8_t binary)
{
    stringstream oStr;
    oStr << (unsigned short)binary;
    return oStr.str();
}

/******************************************************************************************
 * 		Method:			InstructionFetch::GetImmediateValue
 *
 * 		Parameters:     uint16_t: 16 bit immediate value
 *                      bool: if immediate value is unsigned (default to false)
 * 		Return:         string: decimal immediate value
 * 		Description:    converts signed or unsigned immediate value to decimal string
 ******************************************************************************************/
string InstructionFetch::GetImmediateValue(uint16_t binary, bool unsignedValue)
{
    stringstream oStr;
    oStr << "#";
    bool negative = ( binary & (1<<15) );
    if(negative && !unsignedValue)
    {
        oStr << "-";
        binary = (~binary) + 1;
        oStr << binary;
    }
    else
        oStr << binary;
    return oStr.str();
}

/*****************************************************************************************************
 * 		Method:			InstructionFetch::GetMemoryOffset
 *
 * 		Parameters:     uint16_t: 16 bit offset value
 * 		Return:         string: decimal offset value
 * 		Description:    converts 16 bit binary offset value from memory instruction to decimal string
 *****************************************************************************************************/
string InstructionFetch::GetMemoryOffset(uint16_t binary)
{
    stringstream oStr;
    bool negative = ( binary & (1<<15) );
    if(negative)
    {
        oStr << "-";
        binary = (~binary) + 1;
    }
    oStr << binary;
    return oStr.str();
}

/***********************************************************************************************************
 * 		Method:			InstructionFetch::GetBranchOffset
 *
 * 		Parameters:     uint16_t: 16 bit offset value
 * 		Return:         string: decimal offset value
 * 		Description:    converts 16 bit binary offset value from branch instruction to decimal string
 ***********************************************************************************************************/
string InstructionFetch::GetBranchOffset(uint16_t binary)
{
    stringstream oStr;
    bool negative = ( binary & (1<<15) );
    if(negative)
    {
        oStr << "-";
        binary = (~binary) + 1;
    }
    binary*=4;
    oStr << binary;
    branchDestination = programCounter + 4 + binary;
    return oStr.str();
}


