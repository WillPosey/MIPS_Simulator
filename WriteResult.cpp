/************************************************
 * 		WriteResult.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#include "WriteResult.h"

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
    string name;
    CDB_Entry cdbEntry;

    cdbWrite.clear();
    cdbEntry.type = rob;

    for(int i=0; i<10; i++)
    {
        currentRS = RS.GetEntry(i);
        if(currentRS.busy && currentRS.hasExecuted && !currentRS.hasWritten)
        {
            type = currentRS.instruction.info.type;
            name = currentRS.instruction.info.name;

            if(type == SPECIAL || type == IMMEDIATE || (!name.compare("LW")) )
            {
                currentRS.hasWritten = true;
                RS.SetEntry(i, currentRS);
                cdbEntry.destination = currentRS.robDest;
                cdbEntry.value = currentRS.result;
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
        currentROB = ROB.GetEntry(cdbEntry->destination);
        currentROB.value = cdbEntry->value;
        currentROB.state = Cmt;
        ROB.SetEntry(cdbEntry->destination, currentROB);
    }

    if(cdbWrite.size() > 0)
        CDB.Write(cdbWrite);
}

