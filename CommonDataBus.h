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

/* types of data on CDB */
typedef enum
{
    rob,
    address,
    storeReady,
    mispredict
} CDB_Type;

/* entry on CDB */
struct CDB_Entry
{
    CDB_Type type;
    int destination;
    int value;
};

/* RS operands listened for on CDB */
typedef enum
{
    RS_j,
    RS_k
} RS_Operand;

/* data listened for on CDB */
struct ListenCDB
{
    int destination;
    int rsNum;
    RS_Operand operand;
};

/* CommonDataBus class declaration */
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
