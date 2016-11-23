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

using namespace std;

static const string robNames[] = {"ROB0", "ROB1", "ROB2", "ROB3", "ROB4", "ROB5"};

typedef enum
{
    Execute,
    WriteResult,
    Commit
} ROB_State;

typedef enum
{
    Register,
    Memory
} DestinationType;

struct ROB_Entry
{
    bool busy;
    Instruction instruction;
    ROB_State state;
    DestinationType destination;
    int destinationAddress;
    bool addressPresent;
    int value;

    ROB_Entry():    busy(false),
                    state(Execute),
                    destination(Register),
                    destinationAddress(0),
                    addressPresent(false),
                    value(0)
                    {}
};

class ReorderBuffer
{
public:
    ReorderBuffer(){numEntries = robHead = robNextEntry = 0;}

    /* Operator overload for setting ROB entry (ReorderBuffer[entryNum] = entry;) */
    ROB_Entry& operator[] (unsigned int entryNum)
    {
        if(entryNum > 5)
            return rob[5];

        return rob[entryNum];
    }

    /* Operator overload for getting ROB entry (entry = ReorderBuffer[entryNum];) */
    ROB_Entry operator[] (unsigned int entryNum) const
    {
        if(entryNum > 5)
            return rob[5];

        return rob[entryNum];
    }

    bool Available(){return (numEntries < 6);}
    int CreateEntry(ROB_Entry newEntry);
    void UpdateEntry(int entryNum, ROB_State state, int value = 0, int destAddress = 0);
private:
    void ClearEntry(int entryNum);

    ROB_Entry rob[NUM_ROB_ENTRIES];
    int numEntries;
    int robHead;
    int robNextEntry;
};

#endif /* REORDER_BUF_H */
