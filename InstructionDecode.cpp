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
                rsEntry.cycleNum = 0;
                rsEntry.busy = true;
                GetOperands();
                GetDestination();
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
        if(robEntry.destination == Reg)
        {
            RF[robEntry.destinationAddress].busy = true;
            RF[robEntry.destinationAddress].robNumber = rsEntry.robDest;
        }
    }
}

/**************************************************************
 *
 * 		InstructionDecode::GetOperands
 *
 **************************************************************/
void InstructionDecode::GetOperands()
{
    int rs = currentInstruction.info.rs;
    int rt = currentInstruction.info.rt;
    int rd = currentInstruction.info.rd;
    int immVal = currentInstruction.info.immVal;
    string name = currentInstruction.info.name;
    int robNum;

    switch(currentInstruction.info.type)
    {
        case MEMORY:
            /* VJ, QJ*/
            if(!RF[rs].busy)
            {
                rsEntry.Qj = 0;
                rsEntry.Vj = RF[rs].value;
            }
            else
            {
                robNum = RF[rs].robNumber;
                if(ROB[robNum].state == Cmt)
                {
                    rsEntry.Qj = 0;
                    rsEntry.Vj = ROB[robNum].value;
                }
                else
                    rsEntry.Qj = robNum;
            }
            rsEntry.address = currentInstruction.info.offset;
            rsEntry.Qk = 0;
            break;
        case IMMEDIATE:
            /* VJ, QJ*/
            if(!RF[rs].busy)
            {
                rsEntry.Qj = 0;
                rsEntry.Vj = RF[rs].value;
            }
            else
            {
                robNum = RF[rs].robNumber;
                if(ROB[robNum].state == Cmt)
                {
                    rsEntry.Qj = 0;
                    rsEntry.Vj = ROB[robNum].value;
                }
                else
                    rsEntry.Qj = robNum;
            }
            rsEntry.Qk = 0;
            rsEntry.Vk = currentInstruction.info.immVal;
            break;
        case SPECIAL:
        case BRANCH:
            /* VJ, QJ*/
            if(!RF[rs].busy)
            {
                rsEntry.Qj = 0;
                rsEntry.Vj = RF[rs].value;
            }
            else
            {
                robNum = RF[rs].robNumber;
                if(ROB[robNum].state == Cmt)
                {
                    rsEntry.Qj = 0;
                    rsEntry.Vj = ROB[robNum].value;
                }
                else
                    rsEntry.Qj = robNum;
            }
            /* Vk, Qk*/
            if(!RF[rt].busy)
            {
                rsEntry.Qk = 0;
                rsEntry.Vk = RF[rt].value;
            }
            else
            {
                robNum = RF[rt].robNumber;
                if(ROB[robNum].state == Cmt)
                {
                    rsEntry.Qk = 0;
                    rsEntry.Vk = ROB[robNum].value;
                }
                else
                    rsEntry.Qk = robNum;
            }
            break;
        case JUMP:
            rsEntry.Qj = 0;
            rsEntry.Qk = 0;
            break;
        case REGIMM:
            /* VJ, QJ*/
            if(!RF[rs].busy)
            {
                rsEntry.Qj = 0;
                rsEntry.Vj = RF[rs].value;
            }
            else
            {
                robNum = RF[rs].robNumber;
                if(ROB[robNum].state == Cmt)
                {
                    rsEntry.Qj = 0;
                    rsEntry.Vj = ROB[robNum].value;
                }
                else
                    rsEntry.Qj = robNum;
            }
            rsEntry.Qk = 0;
            break;
    }
}

/**************************************************************
 *
 * 		InstructionDecode::GetDestination
 *
 **************************************************************/
void InstructionDecode::GetDestination()
{
    int rt = currentInstruction.info.rt;
    int rd = currentInstruction.info.rd;
    string name = currentInstruction.info.name;

    switch(currentInstruction.info.type)
    {
        case MEMORY:
            if(!name.compare("SD"))
                robEntry.destination = Mem;
            else
            {
                robEntry.destination = Reg;
                robEntry.destinationAddress = rt;
            }
            break;
        case IMMEDIATE:
            robEntry.destination = Reg;
            robEntry.destinationAddress = rt;
            break;
        case BRANCH:
        case JUMP:
        case REGIMM:
            robEntry.destination = Br;
            break;
        case SPECIAL:
            robEntry.destination = Reg;
            robEntry.destinationAddress = rd;
            break;
    }
}

