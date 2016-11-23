/************************************************
 * 		RegisterFile.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef REG_FILE_H
#define REG_FILE_H

#include <string>
#include <cstring>

using namespace std;

struct RF_Entry
{
    bool busy;
    int robNumber;
    int32_t value;

    RF_Entry(): busy(false),
                robNumber(0),
                value(0)
                {}
};

class RegisterFile
{
public:
    /* Operator overload for setting register value (RegisterFile[registerNum] = value;) */
    RF_Entry& operator[] (unsigned int regNum)
    {
        if(regNum > 31)
            return registerFileContent[31];

        return registerFileContent[regNum];
    }

    /* Operator overload for getting register value (value = RegisterFile[registerNum];) */
    RF_Entry operator[] (unsigned int regNum) const
    {
        if(regNum > 31)
            return registerFileContent[31];

        return registerFileContent[regNum];
    }

    /* Returns content of all registers as a string */
    string GetContents()
    {
        int regNum = 0;
        string content = "Registers:\r\n";

        for(int i=0; i<4; i++)
        {
            regNum = i*8;
            content += "R";
            if(i<2)
                content += "0";
            content += to_string(regNum) + ":\t";

            for(int j=0; j<8; j++)
            {
                content += to_string(registerFileContent[regNum++].value);
                if(j!=8)
                    content += "\t";
            }
            content += "\r\n";
        }
        return content;
    }

private:
    RF_Entry registerFileContent[32];
};

#endif /* REG_FILE_H */
