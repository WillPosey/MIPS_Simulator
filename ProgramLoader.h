/************************************************
 * 		ProgramLoader.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef PROG_LOADER_H
#define PROG_LOADER_H

#include "MainMemory.h"

#include <string>
#include <fstream>

using namespace std;

/* ProgramLoader class declaration */
class ProgramLoader
{
public:
    /************************************************
     *      ProgramLoader Constructor
     ************************************************/
    ProgramLoader (string filename, MainMemory& memory)
    {
        uint8_t byte;
        int byteNum;
        int memVal;

        /* Open the MIPS binary input file */
        ifstream binStream (filename.c_str(), ios::binary);
        if(binStream)
        {
            /* Determine what the last address will be */
            binStream.seekg (0, binStream.end);
            int totalBytes = binStream.tellg();
            binStream.seekg (0, binStream.beg);
            int lastAddress = totalBytes + ADDRESS_START - 4;

            /* Fill memory with instructions and data */
            for(int address=ADDRESS_START; address<=lastAddress; address+=4)
            {
                memVal = 0;
                for(int byteNum=3; byteNum>=0; byteNum--)
                {
                    binStream.read((char*)&byte, 1);
                    memVal |= (byte<<(byteNum*8));
                }
                memory.SetValue(address, memVal);
            }
            binStream.close();
        }
    }
};

#endif /* PROG_LOADER_H */
