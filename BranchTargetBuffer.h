/************************************************
 * 		BranchTargetBuffer.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef BTB_H
#define BTB_H

#define NUM_BTB_ENTRIES 16

#include <string>
#include <unordered_map>
#include <cstddef>

struct BTB_Entry
{
    int srcAddress;
    int destAddress;
    bool prediction;
    BTB_Entry* left;
    BTB_Entry* right;

    BTB_Entry(int src, int dest, bool predict): srcAddress(src),
                                                destAddress(dest),
                                                prediction(predict),
                                                left(this),
                                                right(this)
                                                {}
}

using namespace std;

class BranchTargetBuffer
{
public:
    bool Hit(int srcAddress);
    void CreateEntry(int srcAddress, int destAddress, bool prediction);
    void UpdatePrediction(int srcAddress, bool prediction);
    string GetContent();
private:
    BTB_Entry* RemoveLRU();
    void PlaceMRU(BTB_Entry* accessedEntry);

    unordered_map<int,BTB_Entry*> hashTable;
    unordered_map<int,BTB_Entry*> indexHashTable;   // used to print in order
    BTB_Entry* LRU;
    BTB_Entry* MRU;
    int numEntries;
};

#endif /* BTB_H */
