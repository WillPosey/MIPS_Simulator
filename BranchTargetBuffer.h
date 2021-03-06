/************************************************
 * 		BranchTargetBuffer.h
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
#ifndef BTB_H
#define BTB_H

#include <string>
#include <unordered_map>
#include <cstddef>

using namespace std;

/* Branch Target Buffer Entry Struct Definition */
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
                                                left(NULL),
                                                right(NULL)
                                                {}
};

/* BranchTargetBuffer class declaration */
class BranchTargetBuffer
{
public:
    BranchTargetBuffer();
    bool Hit(int srcAddress);
    bool GetPrediction(int srcAddress);
    int GetDestinationAddress(int srcAddress);
    void CreateEntry(int srcAddress, int destAddress, bool prediction);
    void UpdatePrediction(int srcAddress, int destAddress, bool prediction);
    string GetContent();

private:
    bool EntryExists(int srcAddress);
    string GetEntryString(BTB_Entry* entry);
    BTB_Entry* RemoveLRU();
    void UpdateMRU(int srcAddress);
    void PlaceMRU(BTB_Entry* accessedEntry);

    unordered_map<int,BTB_Entry*> hashTable;
    unordered_map<BTB_Entry*,int> indexHashTable;
    unordered_map<int,BTB_Entry*> reverseIndexHashTable;
    BTB_Entry* LRU;
    BTB_Entry* MRU;
    int numEntries;
};

#endif /* BTB_H */
