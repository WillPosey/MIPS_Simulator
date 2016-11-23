/************************************************
 * 		InstructionDecode.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#include "InstructionDecode.h"

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

/**************************************************************
 *
 * 		InstructionDecode Constructor
 *
 **************************************************************/
InstructionDecode::InstructionDecode(InstructionQueue& iqRef, ReservationStation& rsRef, ReorderBuffer& robRef, RegisterFile& rfRef, CommonDataBus& cdbRef)
:   IQ(iqRef),
    RS(rsRef),
    ROB(robRef),
    RF(rfRef),
    CDB(cdbRef)
{

}

/**************************************************************
 *
 * 		InstructionDecode::RunCycle
 *
 **************************************************************/
void InstructionDecode::RunCycle()
{
    stall = false;
    nop_break = false;
    if(IQ.NotEmpty())
    {
        /* Peek at instruction */
        currentInstruction = IQ.Read();

        /* NOP and BREAK only need a ROB entry */
        if(currentInstruction.info.name.compare("NOP") == 0 || currentInstruction.info.name.compare("BREAK") == 0)
        {
            if(ROB.Available())
            {
                nop_break = true;
                IQ.Pop();
                robEntry.busy = true;
                robEntry.instruction = currentInstruction;
                robEntry.state = Cmt;
            }
            else
                stall = true;
        }
        /* Reserve RS and ROB entries, if avilable */
        else
        {
            if(RS.Available() && ROB.Available())
            {
                IQ.Pop();
                robEntry.busy = true;
                robEntry.instruction = currentInstruction;
                robEntry.state = Ex;
                rsEntry.instruction = currentInstruction;
                /* determine operands, destination, etc */
            }
            else
                stall = true;
        }
    }
    else
        stall = true;
}

/**************************************************************
 *
 * 		InstructionDecode::CompleteCycle
 *
 **************************************************************/
void InstructionDecode::CompleteCycle()
{
    if(stall)
        return;

    /* Fill out ROB for NOP or BREAK instruction; bypass reservation station */
    if(nop_break)
    {
        ROB.CreateEntry(robEntry);
    }
    /* Fill out reservation station and reorder buffer entries */
    else
    {
        rsEntry.robDest = ROB.CreateEntry(robEntry);
        RS.CreateEntry(rsEntry);
    }

}

