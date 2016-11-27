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
Execute::Execute(MainMemory& memRef, RegisterFile& rfRef, BranchTargetBuffer& btbRef, ReservationStation& rsRef, ReorderBuffer& robRef, CommonDataBus& cdbRef)
:   memory(memRef),
    RF(rfRef),
    BTB(btbRef),
    RS(rsRef),
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

    if(!RS[rsIndex].Qj)
        VjAvail = true;
    else
    {
        listenForOperand.destination = RS[rsIndex].Qj;
        listenForOperand.operand = RS_j;
        listenForOperand.rsNum = rsIndex;
        cdbListen.push_back(listenForOperand);
    }

    if(!RS[rsIndex].Qk)
        VkAvail = true;
    else
    {
        listenForOperand.destination = RS[rsIndex].Qk;
        listenForOperand.operand = RS_k;
        listenForOperand.rsNum = rsIndex;
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
    int rt = RS[rsIndex].instruction.info.rt;
    int binary = RS[rsIndex].instruction.binary;
    bool negative = ( binary & (1<<15) );
    int PC = RS[rsIndex].instruction.PC;
    int Vj = RS[rsIndex].Vj;
    int Vk = RS[rsIndex].Vk;

    if(cycleNum == 0)
    {
        switch(type)
        {
            case MEMORY:
                /* all addresses before this have been calculated */
                if(ROB.CheckAddressCalc(robNum))
                {
                    executeResult.type = addressCalc;
                    executeResult.rsIndex = rsIndex;
                    executeResult.value = RS[rsIndex].address + Vj;
                    completeEx.push_back(executeResult);
                }
                return;
            case IMMEDIATE:
                executeResult.type = alu;
                executeResult.rsIndex = rsIndex;
                if(!name.compare("SLTI"))
                    executeResult.value = (Vj < Vk) ? 1 : 0;
                else if(!name.compare("ADDI"))
                    executeResult.value = Vj + Vk;
                else if(!name.compare("ADDIU"))
                    executeResult.value = (unsigned) Vj + (unsigned) Vk;
                else
                    return;
                completeEx.push_back(executeResult);
                return;
            case BRANCH:
                executeResult.type = branchOutcome;
                executeResult.rsIndex = rsIndex;
                if(negative)
                    binary = (~binary) + 1;
                binary*=4;
                executeResult.address = PC + 4 + binary;
                if(!name.compare("BEQ"))
                    executeResult.value = (Vj == Vk) ? 1 : 0;
                else if(!name.compare("BNE"))
                    executeResult.value = (Vj != Vk) ? 1 : 0;
                else if(!name.compare("BLEZ"))
                    executeResult.value = (Vj <= 0) ? 1 : 0;
                else if(!name.compare("BGTZ"))
                    executeResult.value = (Vj >= 0) ? 1 : 0;
                else
                    return;
                completeEx.push_back(executeResult);
                return;
            case JUMP:
                executeResult.type = branchOutcome;
                executeResult.rsIndex = rsIndex;
                executeResult.address = ( (binary & JUMP_TARGET_MASK) << 2 ) | (PC & JUMP_PC_MASK);
                executeResult.value = 1;
                completeEx.push_back(executeResult);
                return;
            case REGIMM:
                executeResult.type = branchOutcome;
                executeResult.rsIndex = rsIndex;
                if(negative)
                    binary = (~binary) + 1;
                binary*=4;
                executeResult.address = PC + 4 + binary;
                if(!name.compare("BLTZ"))
                    executeResult.value = (Vj < 0) ? 1 : 0;
                else if(!name.compare("BGEZ"))
                    executeResult.value = (Vj >= 0) ? 1 : 0;
                else
                    return;
                completeEx.push_back(executeResult);
                return;
            case SPECIAL:
                executeResult.type = alu;
                executeResult.rsIndex = rsIndex;
                if(!name.compare("SLL"))
                    executeResult.value = Vk<<Vj;
                else if(!name.compare("SRL"))
                    executeResult.value = Vk>>Vj;
                else if(!name.compare("SRA"))
                {
                    executeResult.value = Vk>>Vj;
                    if(Vk < 0)
                        for(int i=31; i>(31-Vj); i--)
                            executeResult.value |= (1<<i);
                }
                else if(!name.compare("ADD"))
                    executeResult.value = Vk + Vj;
                else if(!name.compare("ADDU"))
                    executeResult.value = (unsigned) Vk + (unsigned) Vj;
                else if(!name.compare("SUB"))
                    executeResult.value = Vk - Vj;
                else if(!name.compare("SUBU"))
                    executeResult.value = (unsigned) Vk - (unsigned) Vj;
                else if(!name.compare("AND"))
                    executeResult.value = Vk & Vj;
                else if(!name.compare("OR"))
                    executeResult.value = Vk | Vj;
                else if(!name.compare("XOR"))
                    executeResult.value = Vk ^ Vj;
                else if(!name.compare("NOR"))
                    executeResult.value = ~(Vk | Vj);
                else if(!name.compare("SLT"))
                    executeResult.value = (Vk < Vj) ? 1 : 0;
                else if(!name.compare("SLTU"))
                    executeResult.value = ((unsigned) Vk < (unsigned) Vj) ? 1 : 0;
                else
                    return;
                completeEx.push_back(executeResult);
                return;
        }
    }
    if(cycleNum == 1 && type == MEMORY)
    {
        if(!name.compare("LD"))
        {
            if(ROB.CheckLoadProceed(robNum, RS[rsIndex].address))
            {
                executeResult.type = ldMem;
                executeResult.rsIndex = rsIndex;
                executeResult.value = memory[RS[rsIndex].address];
            }
        }
        if(!name.compare("SD"))
        {
            executeResult.rsIndex = rsIndex;
            executeResult.type = sdMem;
            executeResult.value = Vj;
            /* Place into ROB ready to commit, value equal to rt */
        }
    }
}

/**************************************************************
 *
 * 		Execute::CompleteCycle
 *
 **************************************************************/
void Execute::CompleteCycle()
{
    ExListenCDB listenForOperand;
    int robNum;

    vector<ExResult>::iterator exResult;
    for(exResult=completeEx.begin(); exResult!=completeEx.end(); exResult++)
    {
        RS[exResult->rsIndex].cycleNum++;
        switch(exResult->type)
        {
            case branchOutcome:
                BTB.UpdatePrediction(RS[exResult->rsIndex].instruction.PC, exResult->address, exResult->value);
                ROB[RS[exResult->rsIndex].robDest].state = Cmt;
                ROB[RS[exResult->rsIndex].robDest].instruction.branchHandle.destination = exResult->address;
                ROB[RS[exResult->rsIndex].robDest].instruction.branchHandle.outcome = exResult->value;
                break;
            case addressCalc:
                RS[exResult->rsIndex].address = exResult->value;
                ROB[RS[exResult->rsIndex].robDest].addressPresent = true;
                /* if a SD addressCalc, if regVal ready or ready in ROB, mark entry in ROB ready to Commit */
                if(!RS[exResult->rsIndex].instruction.info.name.compare("SD"))
                {
                    int rt = RS[exResult->rsIndex].instruction.info.rt;
                    if(!RF[rt].busy)
                    {
                        /* Place into ROB ready to commit, value equal to rt */
                        RS[exResult->rsIndex].result = RF[rt].value;
                        ROB[RS[exResult->rsIndex].robDest].value = RF[rt].value;
                        ROB[RS[exResult->rsIndex].robDest].state = Cmt;
                    }
                    else
                    {
                        robNum = RF[rt].robNumber;
                        if(ROB[robNum].state == Cmt)
                        {
                            /* Place into ROB ready to commit, value equal to ROB entry */
                            RS[exResult->rsIndex].result = ROB[robNum].value;
                            ROB[RS[exResult->rsIndex].robDest].value = ROB[robNum].value;
                            ROB[RS[exResult->rsIndex].robDest].state = Cmt;
                        }
                        /* add to listenCDB */
                        else
                        {
                            RS[exResult->rsIndex].Qj = robNum;
                            listenForOperand.destination = RS[exResult->rsIndex].Qj;
                            listenForOperand.operand = RS_j;
                            listenForOperand.rsNum = exResult->rsIndex;
                            cdbListen.push_back(listenForOperand);
                        }
                    }
                }
                break;
            case alu:
            case ldMem:
                RS[exResult->rsIndex].result = exResult->value;
                ROB[RS[exResult->rsIndex].robDest].state = Wr;
                break;
            case sdMem:
                /* Place into ROB ready to commit, value equal to rt */
                RS[exResult->rsIndex].result = RS[exResult->rsIndex].Vj;
                ROB[RS[exResult->rsIndex].robDest].value = RS[exResult->rsIndex].Vj;
                ROB[RS[exResult->rsIndex].robDest].state = Cmt;
                break;
        }
    }
}

/**************************************************************
 *
 * 		Execute::ReadCDB
 *
 **************************************************************/
void Execute::ReadCDB()
{
    vector<CDB_Entry> cdbData = CDB.Read();
    vector<CDB_Entry>::iterator cdbEntry;
    vector<ExListenCDB>::iterator listen_cdbEntry;

    for(cdbEntry=cdbData.begin(); cdbEntry!=cdbData.end(); cdbEntry++)
    {
        for(listen_cdbEntry=cdbListen.begin(); listen_cdbEntry!=cdbListen.end(); listen_cdbEntry++)
        {
            if(cdbEntry->type == rob)
            {
                if(cdbEntry->destination == listen_cdbEntry->destination)
                {
                    if(listen_cdbEntry->operand == RS_j)
                    {
                        RS[listen_cdbEntry->rsNum].Qj = 0;
                        RS[listen_cdbEntry->rsNum].Vj = cdbEntry->value;
                    }
                    else
                    {
                        RS[listen_cdbEntry->rsNum].Qk = 0;
                        RS[listen_cdbEntry->rsNum].Vk = cdbEntry->value;
                    }
                }
            }
        }
    }
}

