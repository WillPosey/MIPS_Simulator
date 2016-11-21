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
int ReorderBuffer::CreateEntry(string instruction, DestinationType dest, int destAddress)
{
    int createdEntryIndex = robNextEntry;

    ClearEntry(robNextEntry);
    rob[robNextEntry].instruction = instruction;
    rob[robNextEntry].destination = dest;
    rob[robNextEntry].destinationAddress = destAddress;

    numEntries++;
    robNextEntry++;
    if(robNextEntry == NUM_ROB_ENTRIES)
        robNextEntry = 0;

    return createdEntryIndex;
}

/**************************************************************
 *
 * 		ReorderBuffer::ClearEntry
 *
 **************************************************************/
void ReorderBuffer::ClearEntry(int entryNum)
{
    rob[robNextEntry].busy = false;
    rob[robNextEntry].instruction = "";
    rob[robNextEntry].state = Execute;
    rob[robNextEntry].destination = Register;
    rob[robNextEntry].destinationAddress = 0;
    rob[robNextEntry].value = 0;
}

/**************************************************************
 *
 * 		ReorderBuffer::UpdateEntry
 *
 **************************************************************/
void ReorderBuffer::UpdateEntry(int entryNum, ROB_State state, int value = 0, int destAddress = 0)
{

}
