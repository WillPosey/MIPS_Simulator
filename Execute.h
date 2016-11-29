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
#include <vector>

using namespace std;

/* execution result types */
typedef enum
{
    branchOutcome,
    jumpCalc,
    addressCalc,
    alu,
    ldMem,
    sdMem
} ExResultType;

/* contains information regarding the result of an instruction's execution */
struct ExResult
{
    int rsIndex;
    ExResultType type;
    int value;
    int address;
};

/* Execute class declaration */
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

    vector<ListenCDB> cdbListen;
    vector<ExResult> completeEx;
    RS_Entry currentRS;
    ROB_Entry currentROB;

    MainMemory& memory;
    RegisterFile& RF;
    BranchTargetBuffer& BTB;
    ReservationStation& RS;
    ReorderBuffer& ROB;
    CommonDataBus& CDB;
};

#endif /* EXECUTE_H */
