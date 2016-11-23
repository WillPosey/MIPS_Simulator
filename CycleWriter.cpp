/************************************************
 * 		CycleWriter.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#include "CycleWriter.h"

#include <cstdlib>

using namespace std;

/**************************************************************
 *
 * 		CycleWriter Constructor
 *
 **************************************************************/
CycleWriter::CycleWriter(string filename)
{
    outStream.open(filename);
    traceAll = false;
    traceStart = traceEnd = 0;
}

/**************************************************************
 *
 * 		CycleWriter::SetTraceParameter
 *
 **************************************************************/
void CycleWriter::SetTraceParameter(string traceParam)
{
    if(traceParam.compare("all") == 0)
    {
        traceAll = true;
        return;
    }

    int firstCycleStart, firstCycleLength, separatorIndex, secondCycleStart, secondCycleLength;
    separatorIndex = traceParam.find(':');
    firstCycleStart = traceParam.find('T')+1;
    firstCycleLength = separatorIndex - firstCycleStart;
    secondCycleStart = separatorIndex+1;
    secondCycleLength = traceParam.length() - secondCycleStart;

    traceStart = atoi(traceParam.substr(firstCycleStart,firstCycleLength).c_str());
    traceEnd = atoi(traceParam.substr(secondCycleStart,secondCycleLength).c_str());
}

/**************************************************************
 *
 * 		CycleWriter::WriteCycle
 *
 **************************************************************/
void CycleWriter::WriteCycle(InstructionQueue IQ, ReservationStation RS, ReorderBuffer ROB, BranchTargetBuffer BTB, RegisterFile RF, MainMemory memory, bool finalCycle)
{
    static int cycleNum = 0;
    cycleNum++;

    string cycleContent;

    if(finalCycle)
        cycleContent = "Final Cycle <" + to_string(cycleNum) + ">:\r\n";
    else
        cycleContent = "Cycle <" + to_string(cycleNum) + ">:\r\n";

    cycleContent += IQ.GetContents();
    cycleContent += RS.GetContent();
    cycleContent += ROB.GetContent();
    cycleContent += BTB.GetContent();
    cycleContent += RF.GetContents();
    cycleContent += memory.GetDataContent();

    if(traceAll)
        outStream << cycleContent;
    else if(traceStart == 0 && traceEnd == 0)
    {
        if(finalCycle)
            outStream << cycleContent;
    }
    else if(traceStart <= cycleNum && cycleNum <= traceEnd)
        outStream << cycleContent;
}
