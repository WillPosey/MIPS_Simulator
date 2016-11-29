/************************************************
 * 		ReorderBuffer.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#include "ReorderBuffer.h"

using namespace std;

/**************************************************************
 *
 * 		ReorderBuffer::CreateEntry
 *
 **************************************************************/
int ReorderBuffer::CreateEntry(ROB_Entry newEntry)
{
    if(numEntries < 6)
    {
        int newEntryNum = robNextEntry;
        rob[robNextEntry-1] = newEntry;
        numEntries++;
        robNextEntry = (robNextEntry==6) ? 1 : robNextEntry+1;

        return newEntryNum;
    }
    return -1;
}

/**************************************************************
 *
 * 		ReorderBuffer::GetEntryByDestination
 *
 **************************************************************/
int ReorderBuffer::GetEntryByDestination(int regNum)
{
    int robNum = -1;
    int curIndex = robHead;
    int counter = 0;

    do
    {
        counter++;
        if(rob[curIndex].destination == Reg && rob[curIndex].destinationAddress == regNum)
        {
            robNum = curIndex+1;
            break;
        }
        curIndex = (curIndex==5) ? 0 : curIndex+1;
    }while(counter <= numEntries);

    return robNum;
}

/**************************************************************
 *
 * 		ReorderBuffer::CheckAddressCalc
 *
 **************************************************************/
bool ReorderBuffer::CheckAddressCalc(int robNum)
{
    if(robNum == robHead)
        return true;

    bool allBeforeCalculated = true;
    int curIndex, stopIndex;

    curIndex = (robNum==1) ? 5 : robNum-2;
    stopIndex = ((robHead-2) < 0) ? 5 : robHead-2;

    do
    {
        if( (rob[curIndex].busy) && (rob[curIndex].instruction.info.type == MEMORY) && (!rob[curIndex].addressPresent) )
        {
            allBeforeCalculated = false;
            break;
        }
        curIndex = (curIndex==0) ? 5 : curIndex-1;
    }while(curIndex!=stopIndex);

    return allBeforeCalculated;
}

/**************************************************************
 *
 * 		ReorderBuffer::CheckLoadProceed
 *
 **************************************************************/
bool ReorderBuffer::CheckLoadProceed(int robNum, int address)
{
    if(robNum == robHead)
        return true;

    bool storeComplete = true;
    int curIndex, stopIndex;

    curIndex = (robNum==1) ? 5 : robNum-2;
    stopIndex = ((robHead-2) < 0) ? 5 : robHead-2;

    do
    {
        if(!rob[curIndex].instruction.info.name.compare("SD"))
        {
            if(rob[curIndex].destinationAddress == address)
                storeComplete = false;
        }
        curIndex = (curIndex==0) ? 5 : curIndex-1;
    }while(curIndex!=stopIndex);

    return storeComplete;
}

/**************************************************************
 *
 * 		ReorderBuffer::ClearEntry
 *
 **************************************************************/
void ReorderBuffer::ClearEntry(int entryNum)
{
    numEntries--;
    rob[entryNum-1].busy = false;
    if(robHead == entryNum)
        robHead = (robHead==6) ? 1 : robHead+1;
}

/**************************************************************
 *
 * 		ReorderBuffer::ClearAll
 *
 **************************************************************/
void ReorderBuffer::ClearAll()
{
    numEntries = 0;
    robNextEntry = 1;
    robHead = 1;
    for(int i=0; i<6; i++)
        rob[i].busy = false;
}

/**************************************************************
 *
 * 		ReorderBuffer::GetContent
 *
 **************************************************************/
string ReorderBuffer::GetContent()
{
    string content = "ROB:\r\n";
    int curIndex = robHead-1;
    for(int i=0; i<numEntries; i++)
    {
        content += "[" + rob[curIndex].instruction.instructionString + "]\r\n";
        curIndex = (curIndex == 5) ? 0 : curIndex+1;
    }
    return content;
}
