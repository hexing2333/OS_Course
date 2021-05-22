#include <iostream>

#include "memoryManager.h"
#include "page.h"

using namespace std;

int main() {
    cout << "ss";
    MemoryManager myM;
    for (int i = 1; i <= 10; i++) {
        //cout << "main" << i << endl;
        cout << "Physical page number: " << i << endl;
        myM.FIFO(i);
    }

    return 0;
}