/************************************************
 * 		Commit.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#include "Commit.h"

#include <iostream>

using namespace std;

/**************************************************************
 *
 * 		Commit Constructor
 *
 **************************************************************/
Commit::Commit(MainMemory& memRef, ReservationStation& rsRef, ReorderBuffer& robRef, RegisterFile& rfRef, CommonDataBus& cdbRef, bool& progComplete)
:   memory(memRef),
    RS(rsRef),
    ROB(robRef),
    RF(rfRef),
    CDB(cdbRef),
    programFinished(progComplete)
{

}

/**************************************************************
 *
 * 		Commit::RunCycle
 *
 **************************************************************/
void Commit::RunCycle()
{
static int cycleCount = 0;
cycleCount++;
    InstructionType type;
    string name;

    readyToCommit = false;

    if(ROB.GetNumEntries() > 0)
    {
        robHeadIndex = ROB.GetHeadNumber();
        type = ROB[robHeadIndex].instruction.info.type;
        name = ROB[robHeadIndex].instruction.info.name;
        if(ROB[robHeadIndex].state == Cmt)
        {
cout << "Cycle " << cycleCount << ": " << ROB[robHeadIndex].instruction.instructionString << endl;
            if(!name.compare("BREAK"))
                commitType = breakCmt;
            else if(!name.compare("NOP"))
                commitType = nopCmt;
            else if(type == SPECIAL || type == IMMEDIATE || !name.compare("LD"))
                commitType = wrReg;
            else if(!name.compare("SD"))
                commitType = wrMem;
            else if(type == BRANCH || type == JUMP || type == REGIMM)
                commitType = brCmt;
            readyToCommit = true;
        }
    }
}

/**************************************************************
 *
 * 		Commit::CompleteCycle
 *
 **************************************************************/
void Commit::CompleteCycle()
{
    CDB_Entry mispredictEntry;
    vector<CDB_Entry> cdbWrite;

    if(readyToCommit)
    {
        int dest = ROB[robHeadIndex].destinationAddress;
        int val = ROB[robHeadIndex].value;

        bool brPredict = ROB[robHeadIndex].instruction.branchHandle.prediction;
        bool brOutcome = ROB[robHeadIndex].instruction.branchHandle.outcome;
        int brDest = ROB[robHeadIndex].instruction.branchHandle.destination;
        int brPC = ROB[robHeadIndex].instruction.PC + 4;

        switch(commitType)
        {
            case breakCmt:
                programFinished = true;
                break;
            case nopCmt:
                ROB.ClearEntry(robHeadIndex);
                break;
            case wrReg:
                RF[dest].value = val;
                RF[dest].busy = false;
                RS.MakeEntryAvailable(robHeadIndex);
                ROB.ClearEntry(robHeadIndex);
                break;
            case wrMem:
                memory[dest] = val;
                RS.MakeEntryAvailable(robHeadIndex);
                ROB.ClearEntry(robHeadIndex);
                break;
            case brCmt:
                if(brPredict != brOutcome)
                {
                    mispredictEntry.type = mispredict;
                    mispredictEntry.value = (brPredict) ? brPC : brDest;
                    cdbWrite.push_back(mispredictEntry);
                    CDB.Write(cdbWrite);
                    ROB.ClearAll();
                    RS.ClearAll();
                }
                else
                {
                    RS.MakeEntryAvailable(robHeadIndex);
                    ROB.ClearEntry(robHeadIndex);
                }
                break;
        }
    }
}

