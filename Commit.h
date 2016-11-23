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
#include "ReservationStation.h"
#include "ReorderBuffer.h"
#include "RegisterFile.h"
#include "CommonDataBus.h"

#include <string>

using namespace std;

class Commit : virtual PipelineStage
{
public:
    Commit(MainMemory& memRef, ReservationStation& rsRef, ReorderBuffer& robRef, RegisterFile& rfRef, CommonDataBus& cdbRef, bool& progComplete);
    void RunCycle();
    void CompleteCycle();
    void ReadCDB();
private:

    MainMemory& memory;
    ReservationStation& RS;
    ReorderBuffer& ROB;
    RegisterFile& RF;
    CommonDataBus& CDB;
    bool& programFinished;
};

#endif /* Commit_H */
