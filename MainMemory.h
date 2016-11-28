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
    int32_t GetValue(int address)
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

    void SetValue(int address, int value)
    {
        int realIndex = (address-ADDRESS_START) / 4.0;
        int lastIndex = memoryContent.size()-1;

        if(realIndex > lastIndex)
        {
            for(int i=0; i < (realIndex - lastIndex); i++)
                memoryContent.push_back(0);
        }

        memoryContent[realIndex] = value;
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
            if(i!=memoryContent.size()-1)
                content += "\t";
            address+=4;
        }
        content += "\r\n";
        return content;
    }

private:
    vector<int32_t> memoryContent;
};

#endif /* MAIN_MEMORY_H */
