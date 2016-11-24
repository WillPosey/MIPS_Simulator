/************************************************
 * 		Execute.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#include "Execute.h"

using namespace std;

/**************************************************************
 *
 * 		Execute Constructor
 *
 **************************************************************/
Execute::Execute(ReservationStation& rsRef, ReorderBuffer& robRef, CommonDataBus& cdbRef)
:   RS(rsRef),
    ROB(robRef),
    CDB(cdbRef)
{

}

/**************************************************************
 *
 * 		Execute::RunCycle
 *
 **************************************************************/
void Execute::RunCycle()
{
    int numRsEntries = RS.GetNumEntries();
    cdbListen.clear();
    cdbWrite.clear();
    completeEx.clear();

    for(int i=0; i<numRsEntries; i++)
        if(CheckOperandsReady(i))
            ExecuteInstruction(i);
}

/**************************************************************
 *
 * 		Execute::CheckOperandsReady
 *
 **************************************************************/
bool Execute::CheckOperandsReady(int rsIndex)
{
    ExListenCDB listenForOperand;
    bool VjAvail, VkAvail;

    VjAvail = VkAvail = false;

    if(!RS[i].Qj)
        VjAvail = true;
    else
    {
        listenForOperand.destination = RS[i].Qj;
        listenForOperand.rsNum = i;
        cdbListen.push_back(listenForOperand);
    }

    if(!RS[i].Qk)
        VkAvail = true;
    else
    {
        listenForOperand.destination = RS[i].Qk;
        listenForOperand.rsNum = i;
        cdbListen.push_back(listenForOperand);
    }

    return (VjAvail && VkAvail);
}

/**************************************************************
 *
 * 		Execute::ExecuteInstruction
 *
 **************************************************************/
void Execute::ExecuteInstruction(int rsIndex)
{
    ExResult executeResult;
    InstructionType type = RS[rsIndex].instruction.info.type;
    string name = RS[rsIndex].instruction.info.name;
    int cycleNum = RS[rsIndex].cycleNum;
    int robNum = RS[rsIndex].robDest;

    switch(type)
    {
        case MEMORY:
            if(cycleNum == 0)
            {
                /* all addresses before this have been calculated */
                if(ROB.CheckAddressCalc(robNum))
                {
                    executeResult.type = addressCalc;
                    executeResult.rsIndex = rsIndex;
                    executeResult.value = RS[rsIndex].address + RS[rsIndex].Vj;
                    completeEx.push_back(executeResult);
                }
            }

            if(!name.compare("LD"))
            {

            }
            if(!name.compare("SD"))
            {

            }
            break;
        case IMMEDIATE:
            if(!name.compare("ADDI"))
            {

            }
            if(!name.compare("ADDIU"))
            {

            }
            if(!name.compare("SLTI"))
            {

            }
            break;
        case BRANCH:
            if(!name.compare("BEQ"))
            {

            }
            if(!name.compare("BNE"))
            {

            }
            if(!name.compare("BLEZ"))
            {

            }
            if(!name.compare("BGTZ"))
            {

            }
            break;
        case JUMP:
            break;
        case REGIMM:
            if(!name.compare("BLTZ"))
            {

            }
            if(!name.compare("BGEZ"))
            {

            }
            break;
        case SPECIAL:
            if(!name.compare("SLL"))
            {

            }
            if(!name.compare("SRL"))
            {

            }
            if(!name.compare("SRA"))
            {

            }
            if(!name.compare("ADD"))
            {

            }
            if(!name.compare("ADDU"))
            {

            }
            if(!name.compare("SUB"))
            {

            }
            if(!name.compare("SUBU"))
            {

            }
            if(!name.compare("AND"))
            {

            }
            if(!name.compare("OR"))
            {

            }
            if(!name.compare("XOR"))
            {

            }
            if(!name.compare("NOR"))
            {

            }
            if(!name.compare("SLT"))
            {

            }
            if(!name.compare("SLTU"))
            {

            }
            break;
    }
}

/**************************************************************
 *
 * 		Execute::CompleteCycle
 *
 **************************************************************/
void Execute::CompleteCycle()
{
    cdbWrite.clear();

    CDB.Write(cdbWrite);
}

/**************************************************************
 *
 * 		Execute::ReadCDB
 *
 **************************************************************/
void Execute::ReadCDB()
{

}

