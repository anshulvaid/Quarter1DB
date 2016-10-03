#ifndef _pfm_h_
#define _pfm_h_

typedef unsigned PageNum;
typedef int RC;
typedef char byte;

#define PAGE_SIZE 4096
#include <string>
#include <climits>
#include <fstream>
using namespace std;

class FileHandle;

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


class FileHandle
{
public:
    // Variables to keep the counter for each operation
    unsigned readPageCounter;
    unsigned writePageCounter;
    unsigned appendPageCounter;

    // Default constructor
    FileHandle();

    // Destructor
    ~FileHandle();

    // Gets a specific page
    RC readPage(PageNum pageNum, void *data);

    // Writes a specific page
    RC writePage(PageNum pageNum, const void *data);

    // Appends a specific page
    RC appendPage(const void *data);

    // Gets the number of pages in the file
    unsigned getNumberOfPages();

    // Puts the current counter values into variables
    RC collectCounterValues(unsigned& readPageCount,
                            unsigned& writePageCount,
                            unsigned& appendPageCount);


private:
    fstream _fs;
};

#endif
