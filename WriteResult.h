/************************************************
 * 		WriteResult.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef WriteResult_H
#define WriteResult_H

#include "PipelineStage.h"
#include "MIPSdefs.h"
#include "ReservationStation.h"
#include "ReorderBuffer.h"
#include "CommonDataBus.h"

#include <string>

using namespace std;

class WriteResult : virtual PipelineStage
{
public:
    WriteResult(ReservationStation& rsRef, ReorderBuffer& robRef, CommonDataBus& cdbRef);
    void RunCycle();
    void CompleteCycle();
    void ReadCDB();
private:

    ReservationStation& RS;
    ReorderBuffer& ROB;
    CommonDataBus& CDB;
};

#endif /* WriteResult_H */
