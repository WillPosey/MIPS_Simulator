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
#include "ReservationStation.h"
#include "ReorderBuffer.h"
#include "CommonDataBus.h"

#include <string>

using namespace std;

struct ExListenCDB
{
    int destination;
    int rsNum;
};

enum
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
};

class Execute : public virtual PipelineStage
{
public:
    Execute(ReservationStation& rsRef, ReorderBuffer& robRef, CommonDataBus& cdbRef);
    void RunCycle();
    void CompleteCycle();
    void ReadCDB();

private:
    void

    vector<CDB_Entry> cdbWrite;
    vector<ExListenCDB> cdbListen;
    ReservationStation& RS;
    ReorderBuffer& ROB;
    CommonDataBus& CDB;
};

#endif /* EXECUTE_H */
