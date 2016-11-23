/************************************************
 * 		ReorderBuffer.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef REORDER_BUF_H
#define REORDER_BUF_H

#include "MIPSdefs.h"

#define NUM_ROB_ENTRIES 6

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

static const string robNames[] = {"ROB0", "ROB1", "ROB2", "ROB3", "ROB4", "ROB5"};

typedef enum
{
    Ex,
    Wr,
    Cmt
} ROB_State;

typedef enum
{
    Reg,
    Mem,
    Br
} DestinationType;

struct ROB_Entry
{
    int entryNum;
    bool busy;
    Instruction instruction;
    ROB_State state;
    DestinationType destination;
    int destinationAddress;
    bool addressPresent;
    int value;

    ROB_Entry():    busy(false),
                    state(Ex),
                    destination(Reg),
                    destinationAddress(0),
                    addressPresent(false),
                    value(0)
                    {}
};

class ReorderBuffer
{
public:
    ReorderBuffer(){numEntries = robNextEntry = 0;}

    /* Operator overload for setting ROB entry (ReorderBuffer[entryNum] = entry;) */
    ROB_Entry& operator[] (unsigned int entryNum)
    {
        if(entryNum > 5)
            return rob[5];

        return rob[robIndexes[entryNum]];
    }

    /* Operator overload for getting ROB entry (entry = ReorderBuffer[entryNum];) */
    ROB_Entry operator[] (unsigned int entryNum) const
    {
        if(entryNum > 5)
            return rob[5];

        int index = robIndexes.at(entryNum);

        return rob[index];
    }

    bool Available(){return (numEntries < 6);}
    int CreateEntry(ROB_Entry newEntry);
    int GetEntryByDestination(int regNum);
    string GetContent();
private:
    void ClearEntry(int entryNum);

    vector<ROB_Entry> rob;
    unordered_map<int,int> robIndexes;  /* maps rob entry num to index in vector (rob head always at vector index 0) */
    unordered_map<int,int> robEntryNums;  /* maps index in vector to rob entry num */
    int numEntries;
    int robNextEntry;
};

#endif /* REORDER_BUF_H */
