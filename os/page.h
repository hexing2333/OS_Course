#pragma once
#ifndef _PAGE_H_
#define _PAGE_H_

class VirtualPage {
private:
    unsigned int virtualNo;
    int physicalNo;

public:
    VirtualPage(unsigned int vNo, int pNo) : virtualNo(vNo), physicalNo(pNo) {}
    VirtualPage() {}
    ~VirtualPage() {}

    void setVirtualNo(unsigned int vNo) { virtualNo = vNo; }
    void setPhysicalNo(int pNo) { physicalNo = pNo; }
    // void setCount(unsigned int c) { count = c; }
    unsigned int getVirtualNo() const { return virtualNo; }
    int getPhysicalNo() const { return physicalNo; }
    // unsigned int getCount() const { return count; }
};

class PhysicalPage {
private:
    unsigned int virtualNo;
    unsigned int physicalNo;

public:
    PhysicalPage(unsigned int vNo, unsigned int pNo)
        : virtualNo(vNo), physicalNo(pNo) {}
    PhysicalPage() {}
    ~PhysicalPage() {}

    void setVirtualNo(unsigned int vNo) { virtualNo = vNo; }
    void setPhysicalNo(unsigned int pNo) { physicalNo = pNo; }
    unsigned int getVirtualNo() const { return virtualNo; }
    unsigned int getPhysicalNo() const { return physicalNo; }
};

#endif