/************************************************
 * 		MIPSsimulator.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 1: MIPS Simulator
 ************************************************/
 #include "MainMemory.h"
 #include "ProgramLoader.h"
 #include "CycleWriter.h"
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

	return 0;
}



