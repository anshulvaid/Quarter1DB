#ifndef _pfm_h_
#define _pfm_h_

typedef unsigned PageNum;
typedef int RC;
typedef char byte;

#define PAGE_SIZE 4096
#include <string>
#include <climits>
#include <fstream>
#include <functional>
#include "FileHandle.h"
using namespace std;

class PagedFileManager
{
friend class PagedFileManagerTest;   // Allows access to test private methods
public:
    // Access to the _pf_manager instance
    static PagedFileManager* instance();

    // Creates a new file
    RC createFile    (const string& fileName);

    // Destroys a file
    RC destroyFile   (const string& fileName);

    //  Opens a file
    RC openFile      (const string& fileName, FileHandle& fileHandle);

    //  Closes a file
    RC closeFile     (FileHandle& fileHandle);

protected:
    // Constructor
    PagedFileManager();

    // Destructor
    ~PagedFileManager();

private:
    static PagedFileManager *_pf_manager;


    // Checks if file exists on the given path (absolute or relative)
    bool fileExists(const string& fileName);


    // Resets the memory allocated for the static instance.
    // Only used for testing, to enable testing multiple features in the same
    // program.
    void reset();
};



#endif
