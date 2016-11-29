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

/* Struct used to represent a register in the RF */
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

/* RegisterFile class declaration */
class RegisterFile
{
public:
    RF_Entry GetReg(int regNum)                 {return registerFileContent[regNum];}
    void SetReg(int regNum, RF_Entry update)    {registerFileContent[regNum] = update;}
    bool IsBusy(int regNum)                     {return registerFileContent[regNum].busy;}
    int GetROB(int regNum)                      {return registerFileContent[regNum].robNumber;}
    int GetValue(int regNum)                    {return registerFileContent[regNum].value;}

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
                content += to_string(registerFileContent[regNum].value);
                regNum++;
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
