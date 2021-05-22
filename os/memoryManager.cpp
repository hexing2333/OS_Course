#include "memoryManager.h"

#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;

MemoryManager::MemoryManager() : instrucTable(INSTRUCTIONNUM) {
    cout << "a";
    srand(0);
    vector<infoStruct>::iterator it = instrucTable.begin();
    vector<unsigned int> ins(INSTRUCTIONNUM);
    unsigned temp = 0;
    cout << "0";

    for (int i = 0; i < INSTRUCTIONNUM; i+=10) {
        ins[i] = temp;
        for (int j = 0; j < 7; j++) {
            ins[i + 1 + j] = ins[i + j] + 1;
            if (INSTRUCTIONNUM - 1 <= ins[i + 1 + j]) {
                ins[i + 1 + j] = 0;
            }
        }
        ins[i + 8] = rand() % (ins[i + 7]);
        ins[i + 9] = ins[i + 8] + 1 + (rand() % (INSTRUCTIONNUM - 1 - ins[i + 8]));
        temp = ins[i + 9] + 1 + (rand() % (INSTRUCTIONNUM - 1 - ins[i + 9]));
    }
    for (int i = 0; i < INSTRUCTIONNUM; i++) {
        instrucTable[i].instruction = ins[i];
        instrucTable[i].page = ins[i] / 10;
        instrucTable[i].offset = ins[i] % 10;
    }
   
    cout << "1";

    ofstream f;
    f.open("instruct.txt");
    it = instrucTable.begin();
    while (it != instrucTable.end()) {
        f << it->instruction << " " << it->page << " " << it->offset << endl;
        *it++;
    }
    f.close();
}

void MemoryManager::init(unsigned int physicalPageNum) {
    unsigned int i;
    unsigned int virtualPageNum =
        (INSTRUCTIONNUM % 10) ? INSTRUCTIONNUM / 10 + 1 : INSTRUCTIONNUM / 10;
    if (!pageTable.empty()) {
        pageTable.clear();
    }
    for (i = 0; i < virtualPageNum; i++) {
        pageTable.push_back(VirtualPage(i, -1));
    }
    while (!freePhysicalPage.empty()) {
        freePhysicalPage.pop();
    }
    for (i = 0; i < physicalPageNum; i++) {
        freePhysicalPage.push(PhysicalPage(0, i));
    }
    if (!usedPhysicalPage.empty()) {
        usedPhysicalPage.clear();
    }
}

void MemoryManager::FIFO(unsigned int physicalPageNum) {
    unsigned i, pageFaultCount = 0;
    VirtualPage vp;
    PhysicalPage pp;

    init(physicalPageNum);

    for (i = 0; i < INSTRUCTIONNUM; i++) {
        vp = pageTable.at(instrucTable.at(i).page);
        if (vp.getPhysicalNo() == -1) {
            pageFaultCount++;
            if (freePhysicalPage.empty()) {
                pp = usedPhysicalPage.at(0);
                usedPhysicalPage.pop_front();
                pageTable.at(pp.getVirtualNo()).setPhysicalNo(-1);
                freePhysicalPage.push(pp);
            }
            pp = freePhysicalPage.front();
            freePhysicalPage.pop();
            pp.setVirtualNo(vp.getVirtualNo());
            usedPhysicalPage.push_back(pp);
            pageTable.at(instrucTable.at(i).page)
                .setPhysicalNo(pp.getPhysicalNo());
        }
    }

    cout << "FIFO: " << (double)(pageFaultCount * 100 / INSTRUCTIONNUM) << "%"
        << endl;
}