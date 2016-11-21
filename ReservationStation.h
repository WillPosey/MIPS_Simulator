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

#include <string>

using namespace std;

struct RS_Entry
{
    bool busy;
    string instruction;
    int Vj;
    int Vk;
    int Qj;
    int Qk;
    int robDest;
    int address;
};

class ReservationStation
{
public:

private:

};

#endif /* RES_STA_H */
