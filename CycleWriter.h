/************************************************
 * 		CycleWriter.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator

 ************************************************/
#ifndef CYCLE_WRITER_H
#define CYCLE_WRITER_H

#include "InstructionQueue.h"
#include "ReservationStation.h"
#include "ReorderBuffer.h"
#include "BranchTargetBuffer.h"
#include "RegisterFile.h"
#include "MainMemory.h"

#include <string>
#include <fstream>

using namespace std;

/* CycleWriter class definition */
class CycleWriter
{
public:
    CycleWriter(int argc, char** argv);
    void SetTraceParameter(string traceParam);
    void WriteCycle(InstructionQueue IQ,
                    ReservationStation RS,
                    ReorderBuffer ROB,
                    BranchTargetBuffer BTB,
                    RegisterFile RF,
                    MainMemory memory,
                    bool finalCycle);
private:
    ofstream outStream;
    int traceStart;
    int traceEnd;
    bool traceAll;
};

#endif /* CYCLE_WRITER_H */
