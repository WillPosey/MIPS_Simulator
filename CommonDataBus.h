/************************************************
 * 		CommonDataBus.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef CMN_DATA_BUS
#define CMN_DATA_BUS

#include "MIPSdefs.h"

#include <string>
#include <vector>

using namespace std;

struct CDB_Entry
{
    string destination;
    int value;
};

class CommonDataBus
{
public:
    /**************************************************************
     * 		CommonDataBus::Clear
     **************************************************************/
    void Clear()
    {
        cdbContents.clear();
    }

    /**************************************************************
     * 		CommonDataBus::Write
     **************************************************************/
    void Write(vector<CDB_Entry> entries)
    {
        vector<CDB_Entry>::iterator it;
        for(it=entries.begin(); it!=entries.end(); it++)
            cdbContents.push_back(*it);
    }

    /**************************************************************
     * 		CommonDataBus::Read
     **************************************************************/
    vector<CDB_Entry> Read()
    {
        return cdbContents;
    }
private:
    vector<CDB_Entry> cdbContents;
};

#endif /* CMN_DATA_BUS */
