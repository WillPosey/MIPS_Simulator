/************************************************
 * 		Commit.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef Commit_H
#define Commit_H

#include "PipelineStage.h"
#include "MIPSdefs.h"
#include "MainMemory.h"
#include "BranchTargetBuffer.h"
#include "ReservationStation.h"
#include "ReorderBuffer.h"
#include "RegisterFile.h"
#include "CommonDataBus.h"

#include <string>

using namespace std;

/* Type of instruction being committed */
typedef enum
{
    wrReg,
    wrMem,
    brCmt,
    jmpCmt,
    nopCmt,
    breakCmt
} CmtType;

/* Commit class declaration */
class Commit : public virtual PipelineStage
{
public:
    Commit(MainMemory& memRef, BranchTargetBuffer& btbRef, ReservationStation& rsRef, ReorderBuffer& robRef, RegisterFile& rfRef, CommonDataBus& cdbRef, bool& progComplete);
    void RunCycle();
    void CompleteCycle();
    void ReadCDB();

private:
    bool readyToCommit;
    int robHeadIndex;
    CmtType commitType;

    RS_Entry currentRS;
    ROB_Entry currentROB;

    MainMemory& memory;
    BranchTargetBuffer& BTB;
    ReservationStation& RS;
    ReorderBuffer& ROB;
    RegisterFile& RF;
    CommonDataBus& CDB;
    bool& programFinished;
};

#endif /* Commit_H */
