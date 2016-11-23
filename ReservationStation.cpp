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
        numEntries++;
        rs.push_back(newEntry);
    }
}

/**************************************************************
 *
 * 		ReservationStation::GetContent
 *
 **************************************************************/
string ReservationStation::GetContent()
{
    string content = "RS:\r\n";
    vector<RS_Entry>::iterator it;
    for(it=rs.begin(); it!=rs.end(); it++)
        content += "[" + it->instruction.instructionString + "]\r\n";
    return content;
}
