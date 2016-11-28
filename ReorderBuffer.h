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
    ReorderBuffer(){numEntries = 0; robHead = 1; robNextEntry = 1;}

    bool Available(){return (numEntries < 6);}
    int CreateEntry(ROB_Entry newEntry);
    int GetEntryByDestination(int regNum);
    ROB_Entry GetEntry(int robNum){return rob[robNum-1];}
    void SetEntry(int robNum, ROB_Entry update){rob[robNum-1] = update;}
    bool CheckAddressCalc(int robNum);
    bool CheckLoadProceed(int robNum, int address);
    int GetNumEntries(){return numEntries;}
    int GetHeadNumber(){return robHead;}
    void ClearEntry(int entryNum);
    void ClearAll();
    string GetContent();

private:
    ROB_Entry rob[6];
    int robHead;
    int numEntries;
    int robNextEntry;
};

#endif /* REORDER_BUF_H */
