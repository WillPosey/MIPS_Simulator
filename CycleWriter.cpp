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
