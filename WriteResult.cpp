/************************************************
 * 		WriteResult.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#include "WriteResult.h"

#include <iostream>

using namespace std;

/**************************************************************
 *
 * 		WriteResult Constructor
 *
 **************************************************************/
WriteResult::WriteResult(ReservationStation& rsRef, ReorderBuffer& robRef, CommonDataBus& cdbRef)
:   RS(rsRef),
    ROB(robRef),
    CDB(cdbRef)
{

}

/**************************************************************
 *
 * 		WriteResult::RunCycle
 *
 **************************************************************/
void WriteResult::RunCycle()
{
    InstructionType type;
    int cycleNum;
    string name;
    CDB_Entry cdbEntry;
    bool readyToWrite;

    cdbWrite.clear();
    cdbEntry.type = rob;

    for(int i=0; i<RS.GetNumEntries(); i++)
    {
        if(RS[i].busy)
        {
            cycleNum = RS[i].cycleNum;
            type = RS[i].instruction.info.type;
            name = RS[i].instruction.info.name;

            readyToWrite =      ( (cycleNum == 1) && (type == SPECIAL || type == IMMEDIATE) )
                            ||  ( (cycleNum == 2) && (!name.compare("LD")) );

            if(readyToWrite)
            {
                cdbEntry.destination = RS[i].robDest;
                cdbEntry.value = RS[i].result;
                cdbWrite.push_back(cdbEntry);
            }
        }
    }
}

/**************************************************************
 *
 * 		WriteResult::CompleteCycle
 *
 **************************************************************/
void WriteResult::CompleteCycle()
{
    vector<CDB_Entry>::iterator cdbEntry;

    for(cdbEntry=cdbWrite.begin(); cdbEntry!=cdbWrite.end(); cdbEntry++)
    {
        ROB[cdbEntry->destination].value = cdbEntry->value;
        ROB[cdbEntry->destination].state = Cmt;
    }

    CDB.Write(cdbWrite);
}

