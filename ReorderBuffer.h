/************************************************
 * 		ReorderBuffer.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef REORDER_BUF_H
#define REORDER_BUF_H

#define NUM_ROB_ENTRIES 6

#include <string>
#include <queue>

using namespace std;

typedef enum
{
    Execute,
    WriteResult,
    Commit;
} ROB_State;

typedef enum
{
    Register,
    Memory;
} DestinationType;

struct ROB_Entry
{
    bool busy;
    string instruction;
    ROB_State state;
    DestinationType destination;
    int destinationAddress;
    int value;

    ROB_Entry():    busy(false),
                    instruction(""),
                    state(Execute),
                    destination(Register),
                    destinationAddress(0),
                    value(0)
                    {}
};

class ReorderBuffer
{
public:
    ReorderBuffer(){numEntries = robHead = robNextEntry = 0;}

    /* Operator overload for setting register value (ReorderBuffer[entryNum] = entry;) */
    ROB_Entry& operator[] (unsigned int entryNum)
    {
        if(regNum > 5)
            return rob[5];

        return rob[entryNum];
    }

    /* Operator overload for getting ROB entry (entry = ReorderBuffer[entryNum];) */
    ROB_Entry operator[] (unsigned int entryNum) const
    {
        if(regNum > 5)
            return rob[5];

        return rob[entryNum];
    }

    bool Available(){return (numEntries < 6);}
    int CreateEntry(string instruction, DestinationType dest, int destAddress);
    void UpdateEntry(int entryNum, ROB_State state, int value = 0, int destAddress = 0);
private:
    void ClearEntry(int entryNum);

    queue<ROB_Entry> rob;
    int numEntries;
    int robHead;
    int robNextEntry;
};

#endif /* REORDER_BUF_H */
