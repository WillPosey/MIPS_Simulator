/************************************************
 * 		InstructionDecode.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef INSTR_DECODE_H
#define INSTR_DECODE_H

#include "PipelineStage.h"
#include "MIPSdefs.h"
#include "InstructionQueue.h"
#include "ReservationStation.h"
#include "ReorderBuffer.h"
#include "RegisterFile.h"
#include "CommonDataBus.h"

#include <string>

using namespace std;

class InstructionDecode : public virtual PipelineStage
{
public:
    InstructionDecode(InstructionQueue& iqRef, ReservationStation& rsRef, ReorderBuffer& robRef, RegisterFile& rfRef, CommonDataBus& cdbRef);
    void RunCycle();
    void CompleteCycle();
    void ReadCDB(){};
private:
    void GetOperands();
    void GetDestination();

    Instruction currentInstruction;
    ROB_Entry robEntry;
    RS_Entry rsEntry;

    bool stall;
    bool nop_break;

    InstructionQueue& IQ;
    ReservationStation& RS;
    ReorderBuffer& ROB;
    RegisterFile& RF;
    CommonDataBus& CDB;
};

#endif /* INSTR_DECODE_H */
