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

    switch(currentInstruction.info.type)
    {
        case MEMORY:
            /* Vj, Qj*/
            if(!RF[rs].busy)
            {
                rsEntry.Qj = 0;
                rsEntry.Vj = RF[rs].value;
            }
            else
                rsEntry.Qj = ROB.GetEntryByDestination(rs);
            /* Vk, Qk*/
            if(!RF[rt].busy)
            {
                rsEntry.Qk = 0;
                rsEntry.Vk = RF[rt].value;
            }
            else
                rsEntry.Qk = ROB.GetEntryByDestination(rt);
            break;
        case IMMEDIATE:
            /* Vk, Qk*/
            rsEntry.Qk = 0;
            rsEntry.Vk = immVal;
            /* Vj, Qj*/
            if(!RF[rs].busy)
            {
                rsEntry.Qj = 0;
                rsEntry.Vj = RF[rs].value;
            }
            else
                rsEntry.Qk = ROB.GetEntryByDestination(rs);
            break;
        case BRANCH:
            /* Vj, Qj*/
            if(!RF[rs].busy)
            {
                rsEntry.Qj = 0;
                rsEntry.Vj = RF[rs].value;
            }
            else
                rsEntry.Qj = ROB.GetEntryByDestination(rs);
            /* Vk, Qk*/
            if( !name.compare("BEQ") || !name.compare("BNE") )
            {
                if(!RF[rt].busy)
                {
                    rsEntry.Qk = 0;
                    rsEntry.Vk = RF[rt].value;
                }
                else
                    rsEntry.Qk = ROB.GetEntryByDestination(rt);
            }
            else
            {
                rsEntry.Qk = 0;
                rsEntry.Vk = 0;
            }
            break;
        case JUMP:
            /* Vj, Qj*/
            rsEntry.Qj = 0;
            rsEntry.Vj = (currentInstruction.binary & JUMP_TARGET_MASK);
            /* Vk, Qk*/
            rsEntry.Qk = 0;
            rsEntry.Vk = currentInstruction.PC;
            break;
        case REGIMM:
            /* Vj, Qj*/
            if(!RF[rs].busy)
            {
                rsEntry.Qj = 0;
                rsEntry.Vj = RF[rs].value;
            }
            else
                rsEntry.Qj = ROB.GetEntryByDestination(rs);
            /* Vk, Qk*/
            rsEntry.Qk = 0;
            rsEntry.Vk = 0;
            break;
        case SPECIAL:
            if( !name.compare("SLL") || !name.compare("SRL") || !name.compare("SRA") )
            {
                /* Vj, Qj*/
                if(!RF[rt].busy)
                {
                    rsEntry.Qj = 0;
                    rsEntry.Vj = RF[rt].value;
                }
                else
                    rsEntry.Qj = ROB.GetEntryByDestination(rt);
                /* Vk, Qk*/
                rsEntry.Qj = 0;
                rsEntry.Vj = (currentInstruction.binary & SPECIAL_SA_MASK)>>SPECIAL_SA_SHIFT;
            }
            else
            {
                /* Vj, Qj*/
                if(!RF[rs].busy)
                {
                    rsEntry.Qj = 0;
                    rsEntry.Vj = RF[rs].value;
                }
                else
                    rsEntry.Qj = ROB.GetEntryByDestination(rs);
                /* Vk, Qk*/
                if(!RF[rt].busy)
                {
                    rsEntry.Qk = 0;
                    rsEntry.Vk = RF[rt].value;
                }
                else
                    rsEntry.Qk = ROB.GetEntryByDestination(rt);
            }
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

    switch(currentInstruction.info.type)
    {
        case MEMORY:
            robEntry.destination = Mem;
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

