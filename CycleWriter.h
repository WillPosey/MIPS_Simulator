/************************************************
 * 		CycleWriter.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef CYCLE_WRITER_H
#define CYCLE_WRITER_H

#include <string>
#include <fstream>

using namespace std;

class CycleWriter
{
public:
    CycleWriter(string filename) : outputFilename(filename) {};
    void SetTraceParameter(string traceParam);
private:
    string outputFilename;
    int traceStart;
    int traceEnd;
    bool traceAll;
};

#endif /* CYCLE_WRITER_H */
