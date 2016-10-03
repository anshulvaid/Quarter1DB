#include <iostream>
#include <vector>
#include <fstream>
#include <functional>
#include <cstdio>
#include <cassert>

#include "../pfm.h"

using namespace std;


// Framework to write custom unit tests for the class PagedFileManager
class PagedFileManagerTest {
public:
    PagedFileManagerTest() {
        add_test(&PagedFileManagerTest::testFileExists);
    }

    void test() {
        PagedFileManager *pfm;
        for (auto t: _tests) {
            pfm = PagedFileManager::instance();
            t(this, pfm);
            pfm->reset();     // Make sure we start every test in a clean state
        }
    }


private:

    void add_test(function<void(PagedFileManagerTest*, PagedFileManager*)> t) {
        _tests.push_back(t);
    }

    // Custom tests go here

    void testFileExists(PagedFileManager* pfm) {
        cout << "***** Testing 'FileExists' *****" << endl;
        assert(pfm != NULL);

        // Test both relative and absolute paths
        vector<string> fileNames = {
            "example.txt",
            "/tmp/example.txt"
        };

        for (auto f: fileNames) {
            ofstream _(f);
            assert(pfm->fileExists(f));
            remove(f.c_str());
            assert(!pfm->fileExists(f));
        }
    }

    vector<function<void(PagedFileManagerTest*, PagedFileManager*)>> _tests;
};


int main() {
    PagedFileManagerTest().test();
    return 0;
}