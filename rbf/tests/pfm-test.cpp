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
        add_test(&PagedFileManagerTest::testOpenFileWithSameFileHandle);
        add_test(&PagedFileManagerTest::testOpenFileWithOtherFileHandle);
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


    void testOpenFileWithSameFileHandle(PagedFileManager* pfm) {
        cout << "***** Testing 'testOpenFileWithSameFileHandle' *****" << endl;

        RC rc;
        string fileName = "example.txt";

        rc = pfm->createFile(fileName);
        assert(rc == 0 && pfm->fileExists(fileName) &&
               "Creating the file should not fail.");

        // Open the file
        FileHandle fileHandle;
        rc = pfm->openFile(fileName, fileHandle);
        assert(rc == 0 && "Opening the file should not fail.");

        // Open file with same file handle
        rc = pfm->openFile(fileName, fileHandle);
        assert(rc == -1 &&
                "Opening the file with same handle should not fail.");

        remove(fileName.c_str());
    }

    void testOpenFileWithOtherFileHandle(PagedFileManager* pfm) {
        cout << "***** Testing 'testOpenFileWithOtherFileHandle' *****"
             << endl;

        RC rc;
        string fileName = "example.txt";

        rc = pfm->createFile(fileName);
        assert(rc == 0 && pfm->fileExists(fileName) &&
               "Creating the file should not fail.");


        FileHandle fileHandle1, fileHandle2;

        // Open the file
        rc = pfm->openFile(fileName, fileHandle1);
        assert(rc == 0 && "Opening the file should not fail.");

        // Open file with same file handle
        rc = pfm->openFile(fileName, fileHandle2);
        assert(rc == 0 &&
                "Opening the file with another handle should not fail.");

        remove(fileName.c_str());
    }

    vector<function<void(PagedFileManagerTest*, PagedFileManager*)>> _tests;
};


int main() {
    PagedFileManagerTest().test();
    return 0;
}