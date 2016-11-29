/************************************************
 * 		ReservationStation.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef RES_STA_H
#define RES_STA_H

#include "MIPSdefs.h"

#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

/* Reservation Station struct definition */
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
    bool hasWritten;
    bool hasExecuted;

    RS_Entry(): busy(false),
                cycleNum(0),
                Vj(0),
                Vk(0),
                Qj(0),
                Qk(0),
                robDest(0),
                address(0),
                result(0),
                hasWritten(false),
                hasExecuted(false)
                {}
};

/* ReservationStation class definition */
class ReservationStation
{
public:
    ReservationStation(){numEntries = 0;}

    bool Available(){return (numEntries < 10);}
    int GetNumEntries(){return numEntries;}
    void CreateEntry(RS_Entry newEntry);
    void MakeEntryAvailable(int robEntryNum);
    void ClearAll();
    string GetContent();
    RS_Entry GetEntry(int index){return rs[index];}
    RS_Entry GetEntryByROB(int robEntryNum, int& rsNum);
    void SetEntry(int index, RS_Entry updated){rs[index] = updated;}

private:
    RS_Entry rs[10];
    unordered_map<int,int> robEntryToIndex;
    vector<int> entryOrder;
    int numEntries;
};

#endif /* RES_STA_H */
