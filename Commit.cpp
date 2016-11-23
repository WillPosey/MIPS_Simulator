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

}

/**************************************************************
 *
 * 		Commit::CompleteCycle
 *
 **************************************************************/
void Commit::CompleteCycle()
{

}

/**************************************************************
 *
 * 		Commit::ReadCDB
 *
 **************************************************************/
void Commit::ReadCDB()
{

}

