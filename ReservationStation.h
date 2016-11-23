/************************************************
 * 		ReservationStation.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef RES_STA_H
#define RES_STA_H

#define NUM_RS_ENTRIES 10

#include "MIPSdefs.h"

#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

struct RS_Entry
{
    bool busy;
    Instruction instruction;
    int cycleNum;
    int Vj;
    int Vk;
    int Qj;
    int Qk;
    int robDest;
    int address;
    int result;
};

class ReservationStation
{
public:
    ReservationStation(){numEntries = 0;}

    /* Operator overload for setting RS entry (ReservationStation[entryNum] = entry;) */
    RS_Entry& operator[] (unsigned int entryNum)
    {
        if(entryNum >= numEntries)
            return rs[numEntries-1];

        return rs[entryNum];
    }

    /* Operator overload for getting RS entry (entry = ReservationStation[entryNum];) */
    RS_Entry operator[] (unsigned int entryNum) const
    {
        if(entryNum >= numEntries)
            return rs[numEntries-1];

        return rs[entryNum];
    }

    bool Available(){return (numEntries < 10);}
    int GetNumEntries(){return numEntries;}
    void CreateEntry(RS_Entry newEntry);
    void MakeEntryAvailable(int robEntryNum);
    string GetContent();

private:
    vector<RS_Entry> rs;
    unordered_map<int,int> robEntryToIndex;
    int numEntries;
};

#endif /* RES_STA_H */
