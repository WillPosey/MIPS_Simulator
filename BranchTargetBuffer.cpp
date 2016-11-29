/************************************************
 * 		BranchTargetBuffer.cpp
 *
 *      Author: William Posey
 *      Course: CDA 5155
 *      Project 2: MIPS Simulator
 ************************************************/
 #include "BranchTargetBuffer.h"

 #include <sstream>

/**************************************************************
 *
 * 		BranchTargetBuffer Constructor
 *
 **************************************************************/
BranchTargetBuffer::BranchTargetBuffer()
{
    numEntries = 0;
    LRU = NULL;
    MRU = NULL;
}

/**************************************************************
 *
 * 		BranchTargetBuffer::Hit
 *
 **************************************************************/
bool BranchTargetBuffer::Hit(int srcAddress)
{
    if(EntryExists(srcAddress))
    {
        UpdateMRU(srcAddress);
        return true;
    }
    else
        return false;
}

/**************************************************************
 *
 * 		BranchTargetBuffer::GetPrediction
 *
 **************************************************************/
bool BranchTargetBuffer::GetPrediction(int srcAddress)
{
    BTB_Entry* entry = hashTable[srcAddress];
    return entry->prediction;
}

/**************************************************************
 *
 * 		BranchTargetBuffer::GetDestinationAddress
 *
 **************************************************************/
int BranchTargetBuffer::GetDestinationAddress(int srcAddress)
{
    BTB_Entry* entry = hashTable[srcAddress];
    return entry->destAddress;
}

/**************************************************************
 *
 * 		BranchTargetBuffer::CreateEntry
 *
 **************************************************************/
void BranchTargetBuffer::CreateEntry(int srcAddress, int destAddress, bool prediction)
{
    BTB_Entry* newEntry = new BTB_Entry(srcAddress, destAddress, prediction);
    hashTable[srcAddress] = newEntry;

    if(numEntries < 16)
    {
        numEntries++;
        indexHashTable[newEntry] = numEntries;
        reverseIndexHashTable[numEntries] = newEntry;
        if(numEntries == 1)
            LRU = newEntry;
    }
    else
    {
        BTB_Entry* oldLRU = RemoveLRU();
        int oldLRU_Index = indexHashTable[oldLRU];
        indexHashTable.erase(oldLRU);
        reverseIndexHashTable.erase(oldLRU_Index);
        indexHashTable[newEntry] = oldLRU_Index;
        reverseIndexHashTable[oldLRU_Index] = newEntry;
    }
    PlaceMRU(newEntry);
}

/**************************************************************
 *
 * 		BranchTargetBuffer::UpdatePrediction
 *
 **************************************************************/
void BranchTargetBuffer::UpdatePrediction(int srcAddress, int destAddress, bool prediction)
{
    if(EntryExists(srcAddress))
        hashTable[srcAddress]->prediction = prediction;
    else
        CreateEntry(srcAddress, destAddress, prediction);
}

/**************************************************************
 *
 * 		BranchTargetBuffer::GetContent
 *
 **************************************************************/
string BranchTargetBuffer::GetContent()
{
    string content = "BTB:\r\n";
    for(int i=1; i<=numEntries; i++)
        content += GetEntryString(reverseIndexHashTable[i]) + "\r\n";
    return content;
}

/**************************************************************
 *
 * 		BranchTargetBuffer::EntryExists
 *
 **************************************************************/
bool BranchTargetBuffer::EntryExists(int srcAddress)
{
    static unordered_map<int, BTB_Entry*>::const_iterator found;

    found = hashTable.find(srcAddress);
    if(found == hashTable.end())
        return false;
    return true;
}

/**************************************************************
 *
 * 		BranchTargetBuffer::GetEntryString
 *
 **************************************************************/
string BranchTargetBuffer::GetEntryString(BTB_Entry* entry)
{
    string entryString = "[Entry "  + to_string(indexHashTable[entry]) + "]<"
                                    + to_string(entry->srcAddress) + ","
                                    + to_string(entry->destAddress) + ","
                                    + to_string(entry->prediction) + ">";
    return entryString;
}

/**************************************************************
 *
 * 		BranchTargetBuffer::RemoveLRU
 *
 **************************************************************/
BTB_Entry* BranchTargetBuffer::RemoveLRU()
{
    /* LRU only removed when BTB full, so its guaranteed that LRU has right pointer */
    BTB_Entry* oldLRU = LRU;
    LRU = LRU->right;
    LRU->left = NULL;
    return oldLRU;
}

/**************************************************************
 *
 * 		BranchTargetBuffer::UpdateMRU
 *
 **************************************************************/
void BranchTargetBuffer::UpdateMRU(int srcAddress)
{
    /* Because UpdateMRU is called for any hit, could happen for nay size of BTB */
    BTB_Entry* accessedEntry = hashTable[srcAddress];

    /* Check if already MRU */
    if(MRU == accessedEntry)
        return;

    /* Check if LRU and not only entry */
    if(LRU == accessedEntry && numEntries > 1)
    {
        accessedEntry->right->left = NULL;
        LRU = accessedEntry->right;
    }
    /* Not LRU, link siblings in list */
    else
    {
        if(accessedEntry->right)
            accessedEntry->right->left = accessedEntry->left;
        if(accessedEntry->left)
            accessedEntry->left->right = accessedEntry->right;
    }
    PlaceMRU(accessedEntry);
}

/**************************************************************
 *
 * 		BranchTargetBuffer::PlaceMRU
 *
 **************************************************************/
void BranchTargetBuffer::PlaceMRU(BTB_Entry* accessedEntry)
{
    BTB_Entry* oldMRU = MRU;
    MRU = accessedEntry;
    if(oldMRU)
        oldMRU->right = accessedEntry;
    MRU->left = oldMRU;
    MRU->right = NULL;
}
