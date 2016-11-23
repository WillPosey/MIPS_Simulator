/************************************************
 * 		PipelineStage.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef PIPELINE_STAGE_H
#define PIPELINE_STAGE_H

#include "MIPSdefs.h"

using namespace std;

class PipelineStage
{
    virtual void RunCycle() = 0;
    virtual void CompleteCycle() = 0;
    virtual void ReadCDB() = 0;
};

#endif /* PIPELINE_STAGE_H */
