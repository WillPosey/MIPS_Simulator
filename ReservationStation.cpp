/************************************************
 * 		ReservationStation.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#include "ReservationStation.h"

using namespace std;

/**************************************************************
 *
 * 		ReservationStation::CreateEntry
 *
 **************************************************************/
void ReservationStation::CreateEntry(RS_Entry newEntry)
{
    if(numEntries < 10)
    {
        rs[nextAvailable] = newEntry;
        rs[nextAvailable].busy = true;
        robEntryToIndex[newEntry.robDest] = nextAvailable;
        numEntries++;
        nextAvailable++;
        if(nextAvailable==10)
            nextAvailable = 0;
    }
}

/**************************************************************
 *
 * 		ReservationStation::MakeEntryAvailable
 *
 **************************************************************/
void ReservationStation::MakeEntryAvailable(int robEntryNum)
{
    rs[robEntryToIndex[robEntryNum]].busy = false;
    robEntryToIndex.erase(robEntryNum);
    numEntries--;
    rsHead++;
    if(rsHead == 10)
        rsHead = 0;
}

/**************************************************************
 *
 * 		ReservationStation::ClearAll
 *
 **************************************************************/
void ReservationStation::ClearAll()
{
    numEntries = 0;
    nextAvailable = 0;
    rsHead = 0;
    for(int i=0; i<10; i++)
        rs[i].busy = false;
}

/**************************************************************
 *
 * 		ReservationStation::GetContent
 *
 **************************************************************/
string ReservationStation::GetContent()
{
    string content = "RS:\r\n";
    int curIndex = rsHead;
    for(int i=0; i<numEntries; i++)
    {
        content += "[" + rs[curIndex].instruction.instructionString + "]\r\n";
        curIndex++;
        if(curIndex == 10)
            curIndex = 0;
    }
    return content;
}
