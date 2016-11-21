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
private:
    vector<uint32_t> memoryContent;
};

#endif /* MAIN_MEMORY_H */
