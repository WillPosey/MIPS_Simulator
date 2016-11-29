/************************************************
 * 		InstructionDecode.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#include "InstructionDecode.h"

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
    cdbListen.clear();
    stall = false;
    nop_break = false;
    if(IQ.NotEmpty())
    {
        /* Peek at instruction */
        currentInstruction = IQ.Read();

        /* NOP and BREAK only need a ROB entry */
        if(!currentInstruction.info.name.compare("NOP") || !currentInstruction.info.name.compare("BREAK"))
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
        /* Reserve RS and ROB entries, if available */
        else if(RS.Available() && ROB.Available())
        {
            IQ.Pop();
            robEntry.addressPresent = false;
            robEntry.busy = true;
            robEntry.instruction = currentInstruction;
            robEntry.state = Ex;
            rsEntry.instruction = currentInstruction;
            rsEntry.cycleNum = 0;
            rsEntry.busy = true;
            rsEntry.hasWritten = false;
            rsEntry.hasExecuted = false;
            GetOperands();
            GetDestination();
        }
        else
            stall = true;
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
    RF_Entry regEntry;

    if(stall)
        return;

    /* Fill out ROB for NOP or BREAK instruction; bypass reservation station */
    if(nop_break)
        ROB.CreateEntry(robEntry);
    /* Fill out reservation station and reorder buffer entries */
    else
    {
        rsEntry.robDest = ROB.CreateEntry(robEntry);
        RS.CreateEntry(rsEntry);
        if(robEntry.destination == Reg)
        {
            regEntry = RF.GetReg(robEntry.destinationAddress);
            regEntry.busy = true;
            regEntry.robNumber = rsEntry.robDest;
            RF.SetReg(robEntry.destinationAddress, regEntry);
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
    ListenCDB cdbListenEntry;
    int robNum;

    switch(currentInstruction.info.type)
    {
        case MEMORY:
            /* VJ, QJ*/
            if(!RF.IsBusy(rs))
            {
                rsEntry.Qj = 0;
                rsEntry.Vj = RF.GetValue(rs);
            }
            else
            {
                robNum = RF.GetROB(rs);
                currentROB = ROB.GetEntry(robNum);
                if(currentROB.state == Cmt)
                {
                    rsEntry.Qj = 0;
                    rsEntry.Vj = currentROB.value;
                }
                else
                {
                    rsEntry.Qj = robNum;
                    cdbListenEntry.operand = RS_j;
                    cdbListenEntry.destination = robNum;
                    cdbListen.push_back(cdbListenEntry);
                }
            }
            rsEntry.address = currentInstruction.info.offset;
            rsEntry.Qk = 0;
            break;
        case IMMEDIATE:
            /* VJ, QJ*/
            if(!RF.IsBusy(rs))
            {
                rsEntry.Qj = 0;
                rsEntry.Vj = RF.GetValue(rs);
            }
            else
            {
                robNum = RF.GetROB(rs);
                currentROB = ROB.GetEntry(robNum);
                if(currentROB.state == Cmt)
                {
                    rsEntry.Qj = 0;
                    rsEntry.Vj = currentROB.value;
                }
                else
                {
                    rsEntry.Qj = robNum;
                    cdbListenEntry.operand = RS_j;
                    cdbListenEntry.destination = robNum;
                    cdbListen.push_back(cdbListenEntry);
                }
            }
            rsEntry.Qk = 0;
            rsEntry.Vk = currentInstruction.info.immVal;
            break;
        case SPECIAL:
        case BRANCH:
            /* VJ, QJ*/
            if(!RF.IsBusy(rs))
            {
                rsEntry.Qj = 0;
                rsEntry.Vj = RF.GetValue(rs);
            }
            else
            {
                robNum = RF.GetROB(rs);
                currentROB = ROB.GetEntry(robNum);
                if(currentROB.state == Cmt)
                {
                    rsEntry.Qj = 0;
                    rsEntry.Vj = currentROB.value;
                }
                else
                {
                    rsEntry.Qj = robNum;
                    cdbListenEntry.operand = RS_j;
                    cdbListenEntry.destination = robNum;
                    cdbListen.push_back(cdbListenEntry);
                }
            }
            /* Vk, Qk*/
            if(!RF.IsBusy(rt))
            {
                rsEntry.Qk = 0;
                rsEntry.Vk = RF.GetValue(rt);
            }
            else
            {
                robNum = RF.GetROB(rt);
                currentROB = ROB.GetEntry(robNum);
                if(currentROB.state == Cmt)
                {
                    rsEntry.Qk = 0;
                    rsEntry.Vk = currentROB.value;
                }
                else
                {
                    rsEntry.Qk = robNum;
                    cdbListenEntry.operand = RS_k;
                    cdbListenEntry.destination = robNum;
                    cdbListen.push_back(cdbListenEntry);
                }
            }
            break;
        case JUMP:
            rsEntry.Qj = 0;
            rsEntry.Qk = 0;
            break;
        case REGIMM:
            /* VJ, QJ*/
            if(!RF.IsBusy(rs))
            {
                rsEntry.Qj = 0;
                rsEntry.Vj = RF.GetValue(rs);
            }
            else
            {
                robNum = RF.GetROB(rs);
                currentROB = ROB.GetEntry(robNum);
                if(currentROB.state == Cmt)
                {
                    rsEntry.Qj = 0;
                    rsEntry.Vj = currentROB.value;
                }
                else
                {
                    rsEntry.Qj = robNum;
                    cdbListenEntry.operand = RS_j;
                    cdbListenEntry.destination = robNum;
                    cdbListen.push_back(cdbListenEntry);
                }
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
            if(!name.compare("SW"))
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

/**************************************************************
 *
 * 		InstructionDecode::ReadCDB
 *
 **************************************************************/
void InstructionDecode::ReadCDB()
{
    vector<CDB_Entry> cdbData = CDB.Read();
    vector<CDB_Entry>::iterator cdbEntry;
    vector<ListenCDB>::iterator listen_cdbEntry;
    int rsNum;

    for(cdbEntry=cdbData.begin(); cdbEntry!=cdbData.end(); cdbEntry++)
    {
        for(listen_cdbEntry=cdbListen.begin(); listen_cdbEntry!=cdbListen.end(); listen_cdbEntry++)
        {
            if(cdbEntry->type == rob && cdbEntry->destination == listen_cdbEntry->destination)
            {
                currentRS = RS.GetEntryByROB(rsEntry.robDest, rsNum);
                if(listen_cdbEntry->operand == RS_j)
                {
                    currentRS.Qj = 0;
                    currentRS.Vj = cdbEntry->value;
                }
                else
                {
                    currentRS.Qk = 0;
                    currentRS.Vk = cdbEntry->value;
                }
                RS.SetEntry(rsNum, currentRS);
            }
        }
    }
}
