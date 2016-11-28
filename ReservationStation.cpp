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
    int nextAvailable;
    if(numEntries < 10)
    {
        for(int i=0; i<10; i++)
        {
            if(!rs[i].busy)
            {
                nextAvailable = i;
                break;
            }
        }

        rs[nextAvailable] = newEntry;
        rs[nextAvailable].busy = true;
        rs[nextAvailable].hasWritten = false;
        rs[nextAvailable].hasExecuted = false;
        robEntryToIndex[newEntry.robDest] = nextAvailable;
        entryOrder.push_back(nextAvailable);
        numEntries++;
    }
}

/**************************************************************
 *
 * 		ReservationStation::MakeEntryAvailable
 *
 **************************************************************/
void ReservationStation::MakeEntryAvailable(int robEntryNum)
{
    int index = robEntryToIndex.at(robEntryNum);
    robEntryToIndex.erase(robEntryNum);
    rs[index].busy = false;
    int orderIndex = 0;
    for(int i=0; i<entryOrder.size(); i++)
    {
        if(entryOrder[i] == index)
            break;
        orderIndex++;
    }
    entryOrder.erase(entryOrder.begin() + orderIndex);
    numEntries--;
}

/**************************************************************
 *
 * 		ReservationStation::GetEntryByROB
 *
 **************************************************************/
RS_Entry ReservationStation::GetEntryByROB(int robEntryNum, int&rsNum)
{
    int index = robEntryToIndex.at(robEntryNum);
    rsNum = index;
    return rs[index];
}

/**************************************************************
 *
 * 		ReservationStation::ClearAll
 *
 **************************************************************/
void ReservationStation::ClearAll()
{
    numEntries = 0;
    for(int i=0; i<10; i++)
        rs[i].busy = false;
    entryOrder.clear();
}

/**************************************************************
 *
 * 		ReservationStation::GetContent
 *
 **************************************************************/
string ReservationStation::GetContent()
{
    string content = "RS:\r\n";
    for(int i=0; i<entryOrder.size(); i++)
        content += "[" + rs[entryOrder[i]].instruction.instructionString + "]\r\n";
    return content;
}
