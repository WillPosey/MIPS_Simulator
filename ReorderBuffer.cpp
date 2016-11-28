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
        robNextEntry++;
        if(robNextEntry == 7)
            robNextEntry = 1;

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
        curIndex++;
        if(curIndex > 5)
            curIndex = 0;
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

    if(robNum == 1)
        curIndex = 5;
    else
        curIndex = robNum-2;

    stopIndex = robHead - 2;
    if(stopIndex < 0)
        stopIndex = 5;

    do
    {
        if(rob[curIndex].busy && rob[curIndex].instruction.info.type == MEMORY)
        {
            if(!rob[curIndex].addressPresent)
            {
                allBeforeCalculated = false;
                break;
            }
        }
        curIndex--;
        if(curIndex < 0)
            curIndex = 5;
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

    if(robNum == 1)
        curIndex = 5;
    else
        curIndex = robNum-2;

    stopIndex = robHead - 2;
    if(stopIndex < 0)
        stopIndex = 5;

    do
    {
        if(!rob[curIndex].instruction.info.name.compare("SD"))
        {
            if(rob[curIndex].destinationAddress == address)
                storeComplete = false;
        }
        curIndex--;
        if(curIndex < 0)
            curIndex = 5;
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
        curIndex++;
        if(curIndex == 6)
            curIndex = 0;
    }
    return content;
}
