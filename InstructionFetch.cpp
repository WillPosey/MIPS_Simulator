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
#include <vector>

using namespace std;

/**************************************************************
 *
 * 		InstructionFetch Constructor
 *
 **************************************************************/
InstructionFetch::InstructionFetch(MainMemory& memRef, InstructionQueue& instrQRef, BranchTargetBuffer& BTBRef, CommonDataBus& CDBRef)
:   memory(memRef),
    IQ(instrQRef),
    BTB(BTBRef),
    CDB(CDBRef)
{
    UpdateProgramCounter(ADDRESS_START);
    breakFound = false;
    lastInstruction = false;
}

/**************************************************************
 *
 * 		InstructionFetch::RunCycle
 *
 **************************************************************/
void InstructionFetch::RunCycle()
{
    if(programCounter >= DATA_START || lastInstruction)
        return;

    GetNextInstruction();
    InstructionType type = currentInstruction.info.type;

    if(type == BRANCH || type == JUMP || type == REGIMM)
        CheckBTB();
    else
        IncrementProgramCounter();
}

/**************************************************************
 *
 * 		InstructionFetch::CompleteCycle
 *
 **************************************************************/
void InstructionFetch::CompleteCycle()
{
    if(programCounter >= DATA_START || lastInstruction)
        return;

    lastInstruction = breakFound;

    WriteToQueue();
}

/**************************************************************
 *
 * 		InstructionFetch::ReadCDB
 *
 **************************************************************/
void InstructionFetch::ReadCDB()
{
    vector<CDB_Entry> cdbData = CDB.Read();
    vector<CDB_Entry>::iterator cdbEntry;

    for(cdbEntry=cdbData.begin(); cdbEntry!=cdbData.end(); cdbEntry++)
    {
        if(cdbEntry->type == mispredict)
        {
            IQ.Flush();
            UpdateProgramCounter(cdbEntry->value);
            breakFound = lastInstruction = false;
        }
    }
}

/**************************************************************
 *
 * 		InstructionFetch::CheckBTB
 *
 **************************************************************/
void InstructionFetch::CheckBTB()
{
    if(BTB.Hit(programCounter))
    {
        if(BTB.GetPrediction(programCounter))
        {
            UpdateProgramCounter(BTB.GetDestinationAddress(programCounter));
            currentInstruction.branchHandle.prediction = true;
        }
        else
        {
            currentInstruction.branchHandle.prediction = false;
            IncrementProgramCounter();
        }
    }
    else
    {
        currentInstruction.branchHandle.prediction = false;
        IncrementProgramCounter();
    }
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
 * 		InstructionFetch::WriteToQueue
 *
 **************************************************************/
void InstructionFetch::WriteToQueue()
{
    IQ.Write(currentInstruction);
}

/**************************************************************
 *
 * 		InstructionFetch::GetNextInstruction
 *
 **************************************************************/
void InstructionFetch::GetNextInstruction()
{
    currentInstruction.binary = 0;
    currentInstruction.binaryString = "";
    currentInstruction.PC = programCounter;
    currentInstruction.instructionString = "";
    currentInstruction.branchHandle.destination = 0;
    currentInstruction.branchHandle.prediction = false;
    currentInstruction.branchHandle.outcome = false;
    currentInstruction.info.name = "";
    currentInstruction.info.opcode = "";
    currentInstruction.info.rd = currentInstruction.info.rt
                               = currentInstruction.info.rs
                               = currentInstruction.info.offset
                               = currentInstruction.info.immVal
                               = -1;

    currentInstruction.binary = memory.GetValue(programCounter);
    GetInstructionInfo();
}

/**************************************************************
 *
 * 		InstructionFetch::GetInstructionInfo
 *
 **************************************************************/
void InstructionFetch::GetInstructionInfo()
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
        bitVal = ( currentInstruction.binary & (1<<bit) ) ? '1' : '0';
        instrString[index] = bitVal;
        index++;
    }
    instrString[32] = '\0';
    currentInstruction.binaryString = string(instrString);
}

/**************************************************************
 *
 * 		InstructionFetch::BinaryStringToInstruction
 *
 **************************************************************/
void InstructionFetch::BinaryStringToInstruction()
{
    string opcode, name;

    opcode = GetOpcode(currentInstruction.binaryString);
    currentInstruction.info.opcode = opcode;
    currentInstruction.info.type = GetInstructionType(opcode);
    name = GetInstructionName(currentInstruction.info.type, opcode, currentInstruction.binaryString, currentInstruction.binary);
    currentInstruction.info.name = name;
    currentInstruction.instructionString = name;

    switch(currentInstruction.info.type)
    {
        case MEMORY:
            currentInstruction.info.rt = (currentInstruction.binary & MEMORY_RT_MASK)>>MEMORY_RT_SHIFT;
            currentInstruction.info.rs = (currentInstruction.binary & MEMORY_BASE_MASK)>>MEMORY_BASE_SHIFT;
            currentInstruction.info.offset = currentInstruction.binary & MEMORY_OFFSET_MASK;
            currentInstruction.instructionString += " " + GetRegister(currentInstruction.info.rt) + ", "
                                                        + GetMemoryOffset(currentInstruction.info.offset) + "("
                                                        + GetRegister(currentInstruction.info.rs) + ")";
            break;
        case IMMEDIATE:
            currentInstruction.info.rt = (currentInstruction.binary & IMMEDIATE_RT_MASK)>>IMMEDIATE_RT_SHIFT;
            currentInstruction.info.rs = (currentInstruction.binary & IMMEDIATE_RS_MASK)>>IMMEDIATE_RS_SHIFT;
            currentInstruction.instructionString += " " + GetRegister(currentInstruction.info.rt) + ", "
                                                        + GetRegister(currentInstruction.info.rs) + ", ";
            if(!name.compare("ADDIU"))
                currentInstruction.instructionString += GetImmediateValue((currentInstruction.binary & IMMEDIATE_VALUE_MASK),true);
            else
                currentInstruction.instructionString += GetImmediateValue(currentInstruction.binary & IMMEDIATE_VALUE_MASK);
            break;
        case BRANCH:
            currentInstruction.info.rs = (currentInstruction.binary & BRANCH_RS_MASK)>>BRANCH_RS_SHIFT;
            currentInstruction.instructionString += " " + GetRegister(currentInstruction.info.rs) + ", ";
            if( !name.compare("BEQ") || !name.compare("BNE") )
            {
                currentInstruction.info.rt = (currentInstruction.binary & BRANCH_RT_MASK)>>BRANCH_RT_SHIFT;
                currentInstruction.instructionString += GetRegister(currentInstruction.info.rt) + ", ";
            }
            currentInstruction.instructionString += "#" + GetBranchOffset(currentInstruction.binary & BRANCH_OFFSET_MASK);
            break;
        case JUMP:
            currentInstruction.instructionString += " " + GetJumpAddress( (currentInstruction.binary & JUMP_TARGET_MASK), programCounter);
            break;
        case REGIMM:
            currentInstruction.info.rs = (currentInstruction.binary & REGIMM_RS_MASK)>>REGIMM_RS_SHIFT;
            currentInstruction.instructionString += " " + GetRegister(currentInstruction.info.rs) + ", #"
                                    + GetBranchOffset(currentInstruction.binary & REGIMM_OFFSET_MASK);
            break;
        case SPECIAL:
            if( !name.compare("BREAK") )
            {
                breakFound = true;
                break;
            }
            if(!name.compare("NOP") )
                break;
            currentInstruction.info.rd = (currentInstruction.binary & SPECIAL_RD_MASK)>>SPECIAL_RD_SHIFT;
            currentInstruction.instructionString += " " + GetRegister(currentInstruction.info.rd) + ", ";
            if( name.compare("SLL") && name.compare("SRL") && name.compare("SRA") )
            {
                currentInstruction.info.rs = (currentInstruction.binary & SPECIAL_RS_MASK)>>SPECIAL_RS_SHIFT;
                currentInstruction.instructionString += GetRegister(currentInstruction.info.rs) + ", ";
            }
            else
                currentInstruction.info.rs = (currentInstruction.binary & SPECIAL_RS_MASK)>>SPECIAL_RS_SHIFT;
            currentInstruction.info.rt = (currentInstruction.binary & SPECIAL_RT_MASK)>>SPECIAL_RT_SHIFT;
            currentInstruction.instructionString += GetRegister(currentInstruction.info.rt);
            if( !name.compare("SLL") || !name.compare("SRL") || !name.compare("SRA") )
                currentInstruction.instructionString += ", #" + GetShiftAmount((currentInstruction.binary & SPECIAL_SA_MASK)>>SPECIAL_SA_SHIFT);
            break;
        default:
            currentInstruction.instructionString = "";
            break;
    }
}

/**********************************************************************************************
 *
 * 		InstructionFetch::GetInstructionType
 *
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
 *
 * 		InstructionFetch::GetInstructionName
 *
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
 *
 * 		InstructionFetch::GetMemoryInstructionName
 *
 **************************************************************************************************/
string InstructionFetch::GetMemoryInstructionName(string opcode)
{
    if(opcode[2] == '1')
        return instructionNames[MEMORY][SW];
    else
        return instructionNames[MEMORY][LW];
}

/**********************************************************************************************************
 *
 * 		InstructionFetch::GetImmediateInstructionName
 *
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
 *
 * 		InstructionFetch::GetBranchInstructionName
 *
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
 *
 * 		InstructionFetch::GetJumpInstructionName
 *
 ******************************************************************************************/
string InstructionFetch::GetJumpInstructionName()
{
    return instructionNames[JUMP][J];
}

/************************************************************************************************************
 *
 * 		InstructionFetch::GetRegimmInstructionName
 *
 ************************************************************************************************************/
string InstructionFetch::GetRegimmInstructionName(string funct)
{
    if(funct[4] == '1')
        return instructionNames[REGIMM][BGEZ];
    else
        return instructionNames[REGIMM][BLTZ];
}

/************************************************************************************************************
 *
 * 		InstructionFetch::GetSpecialInstructionName
 *
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
 *
 * 		InstructionFetch::GetRegister
 *
 ******************************************************************************************/
string InstructionFetch::GetRegister(uint8_t regVal)
{
    return registerNames[regVal];
}

/**********************************************************************************
 *
 * 		InstructionFetch::GetJumpAddress
 *
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
 *
 * 		InstructionFetch::GetShiftAmount
 *
 ******************************************************************************************/
string InstructionFetch::GetShiftAmount(uint8_t binary)
{
    stringstream oStr;
    oStr << (unsigned short)binary;
    return oStr.str();
}

/******************************************************************************************
 *
 * 		InstructionFetch::GetImmediateValue
 *
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
        currentInstruction.info.immVal = binary*-1;
    }
    else
    {
        oStr << binary;
        currentInstruction.info.immVal = binary;
    }
    return oStr.str();
}

/*****************************************************************************************************
 *
 * 		InstructionFetch::GetMemoryOffset
 *
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
 *
 * 		InstructionFetch::GetBranchOffset
 *
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
    return oStr.str();
}


