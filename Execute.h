/************************************************
 * 		Execute.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef EXECUTE_H
#define EXECUTE_H

#include "PipelineStage.h"
#include "MIPSdefs.h"
#include "MainMemory.h"
#include "RegisterFile.h"
#include "BranchTargetBuffer.h"
#include "ReservationStation.h"
#include "ReorderBuffer.h"
#include "CommonDataBus.h"

#include <string>

using namespace std;

typedef enum
{
    RS_j,
    RS_k
} RS_Operand;

struct ExListenCDB
{
    int destination;
    int rsNum;
    RS_Operand operand;
};

typedef enum
{
    branchOutcome,
    addressCalc,
    alu,
    ldMem,
    sdMem
} ExResultType;

struct ExResult
{
    int rsIndex;
    ExResultType type;
    int value;
    int address;
};

class Execute : public virtual PipelineStage
{
public:
    Execute(MainMemory& memRef, RegisterFile& rfRef, BranchTargetBuffer& btbRef, ReservationStation& rsRef, ReorderBuffer& robRef, CommonDataBus& cdbRef);
    void RunCycle();
    void CompleteCycle();
    void ReadCDB();

private:
    bool CheckOperandsReady(int rsIndex);
    void ExecuteInstruction(int rsIndex);

    vector<ExListenCDB> cdbListen;
    vector<ExResult> completeEx;
    MainMemory& memory;
    RegisterFile& RF;
    BranchTargetBuffer& BTB;
    ReservationStation& RS;
    ReorderBuffer& ROB;
    CommonDataBus& CDB;
};

#endif /* EXECUTE_H */
