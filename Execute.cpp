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
    cdbListen.clear();
    completeEx.clear();

    for(int i=0; i<10; i++)
    {
        currentRS = RS.GetEntry(i);
        if(currentRS.busy && !currentRS.hasExecuted)
        {
            if(CheckOperandsReady(i))
                ExecuteInstruction(i);
        }
    }
}

/**************************************************************
 *
 * 		Execute::CheckOperandsReady
 *
 **************************************************************/
bool Execute::CheckOperandsReady(int rsIndex)
{
    ListenCDB listenForOperand;
    bool VjAvail, VkAvail;

    VjAvail = VkAvail = false;

    if(!currentRS.Qj)
        VjAvail = true;
    else
    {
        listenForOperand.destination = currentRS.Qj;
        listenForOperand.operand = RS_j;
        listenForOperand.rsNum = rsIndex;
        cdbListen.push_back(listenForOperand);
    }

    if(!currentRS.Qk)
        VkAvail = true;
    else
    {
        listenForOperand.destination = currentRS.Qk;
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
    InstructionType type = currentRS.instruction.info.type;
    string name = currentRS.instruction.info.name;
    int cycleNum = currentRS.cycleNum;
    int robNum = currentRS.robDest;
    int rt = currentRS.instruction.info.rt;
    int binary = currentRS.instruction.binary & BRANCH_OFFSET_MASK;
    bool negative = ( binary & (1<<15) );
    int PC = currentRS.instruction.PC;
    int Vj = currentRS.Vj;
    int Vk = currentRS.Vk;

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
                    executeResult.value = currentRS.address + Vj;
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
                if(negative)
                    binary*=-1;
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
                executeResult.type = jumpCalc;
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
                if(negative)
                    binary*=-1;
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
        if(!name.compare("LW"))
        {
            if(ROB.CheckLoadProceed(robNum, currentRS.address))
            {
                executeResult.type = ldMem;
                executeResult.rsIndex = rsIndex;
                executeResult.value = memory.GetValue(currentRS.address);
                completeEx.push_back(executeResult);
            }
        }
        if(!name.compare("SW"))
        {
            executeResult.rsIndex = rsIndex;
            executeResult.type = sdMem;
            executeResult.value = Vj;
            completeEx.push_back(executeResult);
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
    ListenCDB listenForOperand;
    int robNum;

    vector<ExResult>::iterator exResult;
    for(exResult=completeEx.begin(); exResult!=completeEx.end(); exResult++)
    {
        currentRS = RS.GetEntry(exResult->rsIndex);
        currentROB = ROB.GetEntry(currentRS.robDest);
        currentRS.cycleNum++;
        switch(exResult->type)
        {
            case branchOutcome:
                BTB.UpdatePrediction(currentRS.instruction.PC, exResult->address, exResult->value);
            case jumpCalc:
                currentRS.hasExecuted = true;
                currentROB.state = Cmt;
                currentROB.instruction.branchHandle.destination = exResult->address;
                currentROB.instruction.branchHandle.outcome = exResult->value;
                RS.SetEntry(exResult->rsIndex, currentRS);
                ROB.SetEntry(currentRS.robDest, currentROB);
                break;
            case addressCalc:
                currentRS.address = exResult->value;
                currentROB.addressPresent = true;
                /* if a SD addressCalc, if regVal ready or ready in ROB, mark entry in ROB ready to Commit */
                if(!currentRS.instruction.info.name.compare("SW"))
                {
                    currentROB.destinationAddress = exResult->value;
                    int rt = currentRS.instruction.info.rt;
                    if(!RF.IsBusy(rt))
                    {
                        /* Place into ROB ready to commit, value equal to rt */
                        currentRS.hasExecuted = true;
                        currentRS.result = RF.GetValue(rt);
                        currentROB.value = RF.GetValue(rt);
                        currentROB.state = Cmt;
                        RS.SetEntry(exResult->rsIndex, currentRS);
                        ROB.SetEntry(currentRS.robDest, currentROB);
                    }
                    else
                    {
                        robNum = RF.GetROB(rt);
                        currentROB = ROB.GetEntry(robNum);
                        if(currentROB.state == Cmt)
                        {
                            /* Place into ROB ready to commit, value equal to ROB entry */
                            currentRS.hasExecuted = true;
                            currentRS.result = currentROB.value;
                            currentROB = ROB.GetEntry(currentRS.robDest);
                            currentROB.destinationAddress = exResult->value;
                            currentROB.value = currentROB.value;
                            currentROB.state = Cmt;
                            RS.SetEntry(exResult->rsIndex, currentRS);
                            ROB.SetEntry(currentRS.robDest, currentROB);
                        }
                        /* add to listenCDB */
                        else
                        {
                            currentRS.Qj = robNum;
                            RS.SetEntry(exResult->rsIndex, currentRS);
                            currentROB = ROB.GetEntry(currentRS.robDest);
                            currentROB.destinationAddress = exResult->value;
                            ROB.SetEntry(currentRS.robDest, currentROB);
                            listenForOperand.destination = currentRS.Qj;
                            listenForOperand.operand = RS_j;
                            listenForOperand.rsNum = exResult->rsIndex;
                            cdbListen.push_back(listenForOperand);
                        }
                    }
                }
                else
                {
                    RS.SetEntry(exResult->rsIndex, currentRS);
                    ROB.SetEntry(currentRS.robDest, currentROB);
                }
                break;
            case alu:
            case ldMem:
                currentRS.hasExecuted = true;
                currentRS.result = exResult->value;
                currentROB.state = Wr;
                RS.SetEntry(exResult->rsIndex, currentRS);
                ROB.SetEntry(currentRS.robDest, currentROB);
                break;
            case sdMem:
                /* Place into ROB ready to commit, value equal to rt */
                currentRS.hasExecuted = true;
                currentRS.result = currentRS.Vj;
                currentROB.value = currentRS.Vj;
                currentROB.state = Cmt;
                RS.SetEntry(exResult->rsIndex, currentRS);
                ROB.SetEntry(currentRS.robDest, currentROB);
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
    vector<CDB_Entry> cdbWrite;
    vector<CDB_Entry> cdbData = CDB.Read();
    vector<CDB_Entry>::iterator cdbEntry;
    vector<ListenCDB>::iterator listen_cdbEntry;

    for(cdbEntry=cdbData.begin(); cdbEntry!=cdbData.end(); cdbEntry++)
    {
        for(listen_cdbEntry=cdbListen.begin(); listen_cdbEntry!=cdbListen.end(); listen_cdbEntry++)
        {
            if(cdbEntry->type == rob && cdbEntry->destination == listen_cdbEntry->destination)
            {
                currentRS = RS.GetEntry(listen_cdbEntry->rsNum);

                if(listen_cdbEntry->operand == RS_j)
                {
                    currentRS.Qj = 0;
                    currentRS.Vj = cdbEntry->value;
                }
                else
                {
                    currentRS.Qk = 0;
                    currentRS.Vk = cdbEntry->value;
                }
                RS.SetEntry(listen_cdbEntry->rsNum, currentRS);

            }
        }
    }
}

