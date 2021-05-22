#pragma once
#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_

#include <deque>
#include <queue>
#include <vector>

#include "page.h"

#define INSTRUCTIONNUM 100

using namespace std;

typedef struct infoStruct {
    unsigned int instruction;
    unsigned int page;
    unsigned int offset;
} infoStruct;

class MemoryManager {
private:
    vector<infoStruct> instrucTable;
    vector<VirtualPage> pageTable;
    queue<PhysicalPage> freePhysicalPage;
    deque<PhysicalPage> usedPhysicalPage;

    void init(unsigned int physicalPageNum);

public:
    MemoryManager();
    ~MemoryManager() {}
    void OPT(unsigned int physicalPageNum);
    void FIFO(unsigned int physicalPageNum);
    void LRU(unsigned int physicalPageNum);
};

#endif