/************************************************
 * 		MIPSsimulator.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 3: MIPS Simulator
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
 * 		MIPS Simulator Main Method
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

	MainMemory memory;                          /* Main memory segment for simulation */
	ProgramLoader loader (argv[1], memory);     /* Load instructions and data values into memory from input file */
	CycleWriter cycleOutput (argc, argv);          /* Set output file name and trace parameter for CycleWriter */

    InstructionQueue IQ;
    BranchTargetBuffer BTB;
    ReservationStation RS;
    ReorderBuffer ROB;
    RegisterFile RF;
    CommonDataBus CDB;
    bool programFinished = false;

    InstructionFetch IF (memory, IQ, BTB, CDB);
    InstructionDecode ID (IQ, RS, ROB, RF, CDB);
    Execute EX (memory, RF, BTB, RS, ROB, CDB);
    WriteResult WR (RS, ROB, CDB);
    Commit CM (memory, BTB, RS, ROB, RF, CDB, programFinished);

    vector<PipelineStage*> pipeline = {&IF, &ID, &EX, &WR, &CM};
    vector<PipelineStage*>::iterator stage;

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



