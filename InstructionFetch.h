/************************************************
 * 		InstructionFetch.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef INSTR_FETCH_H
#define INSTR_FETCH_H

#include "PipelineStage.h"
#include "MIPSdefs.h"
#include "MainMemory.h"
#include "InstructionQueue.h"
#include "BranchTargetBuffer.h"
#include "CommonDataBus.h"

#include <string>

using namespace std;

/* InstructionFetch class definition */
class InstructionFetch : public virtual PipelineStage
{
public:
    InstructionFetch(MainMemory& memRef, InstructionQueue& instrQRef, BranchTargetBuffer& btbRef, CommonDataBus& cdb);
    void RunCycle();
    void CompleteCycle();
    void ReadCDB();
    void UpdateProgramCounter(int PC);

private:
    void CheckBTB();
    void IncrementProgramCounter();
    void GetNextInstruction();
    void WriteToQueue();

    void GetBinaryString();
    void BinaryStringToInstruction();
    void GetInstructionInfo();

    string GetOpcode(string binaryString){return binaryString.substr(0,6);}
	InstructionType GetInstructionType(string opcode);
	string GetInstructionName(InstructionType type, string opcode, string binaryString, uint32_t binaryValue);
	string GetMemoryInstructionName(string opcode);
	string GetImmediateInstructionName(string opcode);
	string GetBranchInstructionName(string opcode);
	string GetJumpInstructionName();
	string GetRegimmInstructionName(string funct);
	string GetSpecialInstructionName(string funct);

	string GetRegister(uint8_t regVal);
	string GetJumpAddress(uint32_t address, uint32_t pcAddress);
	string GetShiftAmount(uint8_t binary);
	string GetImmediateValue(uint16_t binary, bool unsignedValue=false);
	string GetMemoryOffset(uint16_t binary);
	string GetBranchOffset(uint16_t binary);

    MainMemory& memory;
    InstructionQueue& IQ;
    BranchTargetBuffer& BTB;
    CommonDataBus& CDB;
    int programCounter;
    bool breakFound;
    bool lastInstruction;

    Instruction currentInstruction;
};

#endif /* INSTR_FETCH_H */
