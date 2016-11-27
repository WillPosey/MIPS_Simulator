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
        int createdEntryIndex = robNextEntry;
        robIndexes[createdEntryIndex-1] = rob.size();
        robEntryNums[rob.size()] = createdEntryIndex-1;
        rob.push_back(newEntry);

        numEntries++;
        robNextEntry++;
        if(robNextEntry == 7)
            robNextEntry = 1;

        return createdEntryIndex;
    }
    return -1;
}

/**************************************************************
 *
 * 		ReorderBuffer::CreateEntry
 *
 **************************************************************/
int ReorderBuffer::GetEntryByDestination(int regNum)
{
    int robNum = -1;
    vector<ROB_Entry>::iterator robIter;
    for(robIter=rob.begin(); robIter != rob.end(); robIter++)
    {
        if(robIter->destination == Reg && robIter->destinationAddress == regNum)
        {
            robNum = robEntryNums[robIter-rob.begin()];
            break;
        }
    }
    return robNum;
}

/**************************************************************
 *
 * 		ReorderBuffer::CheckAddressCalc
 *
 **************************************************************/
bool ReorderBuffer::CheckAddressCalc(int robNum)
{
    bool allBeforeCalculated = true;

    for(int i=robIndexes.at(robNum)+1; i<rob.size(); i++)
    {
        if(rob[i].instruction.info.type == MEMORY)
        {
            if(!rob[i].addressPresent)
            {
                allBeforeCalculated = false;
                break;
            }
        }
    }

    return allBeforeCalculated;
}

/**************************************************************
 *
 * 		ReorderBuffer::CheckLoadProceed
 *
 **************************************************************/
bool ReorderBuffer::CheckLoadProceed(int robNum, int address)
{
    bool storeComplete = true;

    for(int i=robIndexes.at(robNum)-1; i>=0; i--)
    {
        if(!rob[i].instruction.info.name.compare("SD"))
        {
            if(rob[i].destinationAddress == address)
                storeComplete = false;
        }
    }

    return storeComplete;
}

/**************************************************************
 *
 * 		ReorderBuffer::GetContent
 *
 **************************************************************/
string ReorderBuffer::GetContent()
{
    string content = "ROB:\r\n";
    vector<ROB_Entry>::iterator it;
    for(it=rob.begin(); it!=rob.end(); it++)
        content += "[" + it->instruction.instructionString + "]\r\n";
    return content;
}
