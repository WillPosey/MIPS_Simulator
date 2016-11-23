/************************************************
 * 		MainMemory.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef MAIN_MEMORY_H
#define MAIN_MEMORY_H

#define ADDRESS_START   600
#define DATA_START      716

#include <vector>
#include <cstdint>
#include <string>
#include <iostream>

using namespace std;

class MainMemory
{
public:
    /* Operator overload for setting memory value (MainMemory[address] = value;) */
    /* Ensures that memory that doesn't exist (not in vector) is "allocated" */
    uint32_t& operator[] (int address)
    {
        int realIndex = (address-ADDRESS_START) / 4.0;
        int lastIndex = memoryContent.size()-1;

        if(realIndex > lastIndex)
        {
            for(int i=0; i < (realIndex - lastIndex); i++)
                memoryContent.push_back(0);
        }

        return memoryContent[realIndex];
    }

    /* Operator overload for getting memory value (value = MainMemory[address];) */
    /* Ensures that memory that doesn't exist (not in vector) is "allocated" */
    uint32_t operator[] (int address) const
    {
        int realIndex = (address-ADDRESS_START) / 4.0;
        int lastIndex = memoryContent.size()-1;

        if(realIndex > lastIndex)
            return 0;

        return memoryContent[realIndex];
    }

    /* Returns a string with the integer value at each memory location */
    string GetDataContent()
    {
        int startIndex = ( (DATA_START - ADDRESS_START) / 4.0 );
        int address = DATA_START;

        string content = "Data Segment:\r\n";
        content += to_string(DATA_START) + ":\t";

        for(int i=startIndex; i<memoryContent.size(); i++)
        {
            content += to_string((int32_t) memoryContent[i] );
            if(i!=memoryContent.size())
                content += "\t";
            address+=4;
        }
        content += "\r\n";
        return content;
    }

private:
    vector<uint32_t> memoryContent;
};

#endif /* MAIN_MEMORY_H */
