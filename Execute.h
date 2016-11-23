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

class Execute : public virtual PipelineStage
{
public:
    Execute(ReservationStation& rsRef, ReorderBuffer& robRef, CommonDataBus& cdbRef);
    void RunCycle();
    void CompleteCycle();
    void ReadCDB();

private:
    ReservationStation& RS;
    ReorderBuffer& ROB;
    CommonDataBus& CDB;
};

#endif /* EXECUTE_H */
