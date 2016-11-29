/************************************************
 * 		Commit.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#include "Commit.h"

using namespace std;

/**************************************************************
 *
 * 		Commit Constructor
 *
 **************************************************************/
Commit::Commit(MainMemory& memRef, BranchTargetBuffer& btbRef, ReservationStation& rsRef, ReorderBuffer& robRef, RegisterFile& rfRef, CommonDataBus& cdbRef, bool& progComplete)
:   memory(memRef),
    BTB(btbRef),
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
    InstructionType type;
    string name;

    readyToCommit = false;

    if(ROB.GetNumEntries() > 0)
    {
        robHeadIndex = ROB.GetHeadNumber();
        currentROB = ROB.GetEntry(robHeadIndex);
        type = currentROB.instruction.info.type;
        name = currentROB.instruction.info.name;
        if(currentROB.state == Cmt)
        {
            if(!name.compare("BREAK"))
                commitType = breakCmt;
            else if(!name.compare("NOP"))
                commitType = nopCmt;
            else if(type == SPECIAL || type == IMMEDIATE || !name.compare("LD"))
                commitType = wrReg;
            else if(!name.compare("SW"))
                commitType = wrMem;
            else if(type == BRANCH || type == REGIMM)
                commitType = brCmt;
            else if(type == JUMP)
                commitType = jmpCmt;
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
    RF_Entry regEntry;

    if(readyToCommit)
    {
        int dest = currentROB.destinationAddress;
        int val = currentROB.value;

        bool brPredict = currentROB.instruction.branchHandle.prediction;
        bool brOutcome = currentROB.instruction.branchHandle.outcome;
        int brDest = currentROB.instruction.branchHandle.destination;
        int brPC = currentROB.instruction.PC;

        switch(commitType)
        {
            case breakCmt:
                programFinished = true;
                ROB.ClearEntry(robHeadIndex);
                break;
            case nopCmt:
                ROB.ClearEntry(robHeadIndex);
                break;
            case wrReg:
                regEntry.value = val;
                regEntry.busy = false;
                RF.SetReg(dest, regEntry);
                RS.MakeEntryAvailable(robHeadIndex);
                ROB.ClearEntry(robHeadIndex);
                break;
            case wrMem:
                memory.SetValue(dest, val);
                RS.MakeEntryAvailable(robHeadIndex);
                ROB.ClearEntry(robHeadIndex);
                break;
            case jmpCmt:
                BTB.UpdatePrediction(brPC, brDest, brOutcome);
            case brCmt:
                if(brPredict != brOutcome)
                {
                    mispredictEntry.type = mispredict;
                    mispredictEntry.value = (brPredict) ? brPC + 4 : brDest;
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

/**************************************************************
 *
 * 		Commit::ReadCDB
 *
 **************************************************************/
void Commit::ReadCDB()
{
    CDB.Clear();
}

