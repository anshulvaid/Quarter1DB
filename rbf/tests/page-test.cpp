#include "../rbfm.h"
#include <cassert>
#include <iostream>

void assertEq(int a, int b) {
    if (a != b) {
        cout << "a=" << a << " b=" << b << endl;
        assert(false);
    }
}

void assertEq(char *a, char *b) {
    if (a != b) {
        cout << "a=" << (void *) a << " b=" << (void *) b << endl;
        assert(false);
    }
}


class PageTest: public Page {
public:
    PageTest() : Page() {}

    void run() {
        // Page is empty initially
        assertEq(0, getNumberSlots());
        assertEq(PAGE_SIZE - 4, getFreeSpace());
        assertEq(0, getFreeSpaceOffset());

        // Insert record
        assertEq(_data + PAGE_SIZE - 2, getFreeSpaceOffsetAddr());
        setFreeSpaceOffset(220);
        assertEq(220, getFreeSpaceOffset());

        // RecordEncoder re((char *) data, recordDescriptor);
        // insertRecord()
    }
};

int main(int argc, char const *argv[]) {
    PageTest test;
    test.run();
    return 0;
}