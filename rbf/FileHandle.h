#ifndef _fileHandle_h_
#define _fileHandle_h_


#include <fstream>
#include <string>
#include "pfm.h"

using namespace std;

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

    // Opens file and associates it the stream.
    // Assumes the file already exists.
    RC openFile(const string& fileName);

    // Closes its associated file if any.
    // Flushes out any data that is still in memory.
    RC closeFile();

    // Is associated with a file
    bool isHandlingFile();

private:
    // Does the actual I/O operations to the disk
    fstream _fs;

    // Current number of pages that the file has
    unsigned _nPages;

    // Sets the number of pages according to the file size.
    // Assumes that the file handle is already handling a file.
    unsigned calcNumberPages();

    // Checks the file size.
    // Assumes that the file handle is already handling a file.
    long getFileSize();
};


#endif