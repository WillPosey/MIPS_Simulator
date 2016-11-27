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
    InstructionType type;
    string name;

    readyToCommit = false;

    if(ROB.GetNumEntries() > 0)
    {
        robHeadIndex = ROB.GetHeadNumber();
        type = ROB[robHeadIndex].instruction.type;
        name = ROB[robHeadIndex].instruction.name;
        if(ROB[robHeadIndex].state == Cmt)
        {
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
        switch(commitType)
        {
            case breakCmt:

            case nopCmt:

            case wrReg:

            case wrMem:

            case brCmt:

        }
    }
}

