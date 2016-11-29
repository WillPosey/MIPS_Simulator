/************************************************
 * 		MIPSsimulator.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
 #include "MainMemory.h"
 #include "ProgramLoader.h"
 #include "CycleWriter.h"
 #include "InstructionQueue.h"
 #include "BranchTargetBuffer.h"
 #include "ReservationStation.h"
 #include "ReorderBuffer.h"
 #include "RegisterFile.h"
 #include "InstructionFetch.h"
 #include "InstructionDecode.h"
 #include "Execute.h"
 #include "WriteResult.h"
 #include "Commit.h"

 #include <iostream>

using namespace std;
/**************************************************************
 *
 * 		MIPS Simulator Main Method
 *
 **************************************************************/
int main(int argc, char** argv)
{
    /* Check for command line input */
    if(argc < 3 || argc > 4)
    {
        cout << "Please provide the input file name and output file name" << endl;
        cout << "Optionally provide tracing parameters" << endl;
        return 0;
    }

	MainMemory memory;                                          /* Main memory segment for simulation */
	ProgramLoader loader (argv[1], memory);                     /* Load instructions and data values into memory from input file */
	CycleWriter cycleOutput (argc, argv);                       /* Set output file name and trace parameter for CycleWriter */

    InstructionQueue IQ;                                        /* Instruction Queue for Fetch and Decode Stages */
    BranchTargetBuffer BTB;                                     /* Branch Target Buffer for branch prediction */
    ReservationStation RS;                                      /* Reservation Station used for Tomasulo Algortihm */
    ReorderBuffer ROB;                                          /* Reorder Buffer used for out of order execution and in order commit */
    RegisterFile RF;                                            /* Registers */
    CommonDataBus CDB;                                          /* Common Data Bus used to write from RS to ROB, and ROB to memory/register/BTB */
    bool programFinished = false;                               /* Flag to indicate a BREAK instruction has committed */

    InstructionFetch IF (memory, IQ, BTB, CDB);                 /* Instruction Fetch stage */
    InstructionDecode ID (IQ, RS, ROB, RF, CDB);                /* Instruction Decode/Issue stage */
    Execute EX (memory, RF, BTB, RS, ROB, CDB);                 /* Execution stage */
    WriteResult WR (RS, ROB, CDB);                              /* Write Result stage */
    Commit CM (memory, BTB, RS, ROB, RF, CDB, programFinished); /* Commit stage */

    vector<PipelineStage*> pipeline = {&IF,&ID,&EX,&WR,&CM};    /* 5 stage pipeline */
    vector<PipelineStage*>::iterator stage;                     /* stage iterator */

    /* iterate through pipeline until BREAK is committed */
    /* RunCycle called, no changes made to shared data structures */
    /* CompleteCycle called, outcome of cycle from RunCycle call implemented, data structures modified */
    /* ReadCDB called, data structure modifications from other pipeline stages read, react accordingly */
    /* Write the content of data structure to output file at end of each desired cycle */
    while(!programFinished)
    {
        for(stage=pipeline.begin(); stage!=pipeline.end(); stage++)
            (*stage)->RunCycle();

        for(stage=pipeline.begin(); stage!=pipeline.end(); stage++)
            (*stage)->CompleteCycle();

        for(stage=pipeline.begin(); stage!=pipeline.end(); stage++)
            (*stage)->ReadCDB();

        cycleOutput.WriteCycle(IQ, RS, ROB, BTB, RF, memory, programFinished);
    }

	return 0;
}



