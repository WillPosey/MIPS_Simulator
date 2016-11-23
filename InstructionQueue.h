/************************************************
 * 		InstructionQueue.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef INSTR_QUEUE_H
#define INSTR_QUEUE_H

#include "MIPSdefs.h"

#include <string>
#include <queue>

using namespace std;

class InstructionQueue
{
public:
    /**************************************************************
     * 		InstructionQueue::NotEmpty
     **************************************************************/
    bool NotEmpty()
    {
        return (instrQueue.size() != 0);
    }

    /**************************************************************
     * 		InstructionQueue::Flush
     **************************************************************/
    void Flush()
    {
        for(int i=0; i<instrQueue.size(); i++)
            instrQueue.pop();
    }

    /**************************************************************
     * 		InstructionQueue::Write
     **************************************************************/
    void Write(Instruction fetchedInstr)
    {
        instrQueue.push(fetchedInstr);
    }

    /**************************************************************
     * 		InstructionQueue::Read
     **************************************************************/
    Instruction Read()
    {
        Instruction nextInstruction = instrQueue.front();
        return nextInstruction;
    }

    /**************************************************************
     * 		InstructionQueue::Pop
     **************************************************************/
    void Pop()
    {
        instrQueue.pop();
    }

    /**************************************************************
     * 		InstructionQueue::GetContents
     **************************************************************/
    string GetContents()
    {
        string contents = "IQ:\r\n";
        Instruction temp;
        string curInstr;
        for(int i=0; i<instrQueue.size(); i++)
        {
            temp = instrQueue.front();
            instrQueue.pop();
            contents += "[" + temp.instructionString + "]\r\n";
            instrQueue.push(temp);
        }
        return contents;
    }

private:
    queue<Instruction> instrQueue;
};

#endif /* INSTR_QUEUE_H */
