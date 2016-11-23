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
 #include "InstructionFetch.h"
 #include "RegisterFile.h"
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
	CycleWriter cycleOutput (argv[2]);          /* Set output file name for CycleWriter */
	if(argc == 4)                               /* Set the trace parameters for CycleWriter; default to tracing all cycles */
        cycleOutput.SetTraceParameter(argv[3]);
    else
        cycleOutput.SetTraceParameter("all");

    InstructionQueue IQ;
    BranchTargetBuffer BTB;
    ReservationStation RS;
    ReorderBuffer ROB;
    RegisterFile RF;
    CommonDataBus CDB;
    bool programFinished = false;

    InstructionFetch IF (memory, IQ, BTB, CDB);
    InstructionDecode ID (IQ, RS, ROB, RF, CDB);
    Execute EX (RS, ROB, CDB);
    WriteResult WR(RS, ROB, CDB);
    Commit CM (memory, RS, ROB, RF, CDB, complete);

    vector<PipelineStage> pipeline[5] = {IF, ID, EX, WR, CM};

    while(!programFinished)
    {
        vector<PipelineStage>::iterator it;
        for(it=pipeline.begin(); it!=pipeline.end(); it++)
            *it.RunCycle();
        for(it=pipeline.begin(); it!=pipeline.end(); it++)
            *it.CompleteCycle();
        for(it=pipeline.begin(); it!=pipeline.end(); it++)
            *it.ReadCDB();

        /*
        IF.RunCycle();
        ID.RunCycle();
        EX.RunCycle();
        WR.RunCycle();
        CM.RunCycle();

        IF.CompleteCycle();
        ID.CompleteCycle();
        EX.CompleteCycle();
        WR.CompleteCycle();
        CM.CompleteCycle();

        IF.ReadCDB();
        ID.ReadCDB();
        EX.ReadCDB();
        WR.ReadCDB();
        CM.ReadCDB();
        */

    }

	return 0;
}



