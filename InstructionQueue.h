/************************************************
 * 		InstructionQueue.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef INSTR_QUEUE_H
#define INSTR_QUEUE_H

#include <string>
#include <queue>

using namespace std;

class InstructionQueue
{
public:
    void Write(string fetchedInstr)
    {
        instrQueue.push(fetchedInstr);
    }

    string Read()
    {
        string nextInstruction = instrQueue.front();
        instrQueue.pop();
        return nextInstruction;
    }

    string GetContents()
    {
        string contents = "IQ:\r\n";
        string temp;
        for(int i=0; i<instrQueue.size(); i++)
        {
            temp = instrQueue.front();
            instrQueue.pop();
            contents += "[" + temp + "]";
            if(i!=instrQueue.size())
                contents += "\r\n";
            instrQueue.push(temp);
        }
        return contents;
    }

private:
    queue<string> instrQueue;
};

#endif /* INSTR_QUEUE_H */
