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
        robIndexes[createdEntryIndex] = rob.size();
        rob.push_back(newEntry);

        numEntries++;
        robNextEntry++;
        if(robNextEntry == 6)
            robNextEntry = 0;

        return createdEntryIndex;
    }
    return -1;
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
