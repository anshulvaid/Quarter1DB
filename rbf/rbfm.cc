#include "rbfm.h"
#include <string.h>
#include <math.h>
#include <vector>
#include <iostream>


////////////////////////////////////////////////////////////////////////////
/// Class ByteArray
////////////////////////////////////////////////////////////////////////////


class ByteArray {
public:
    static char *encode(unsigned value) {
        for (int i = 0; i < sizeof(value); ++i) {
            _auxByteArray[i] = value & 0xFF;
            value >>= 8;
        }
        return _auxByteArray;
    }

    static unsigned decode(const char *arr, size_t n) {
        unsigned res = 0;
        for (int i = 0; i < n; ++i) {
            res |= (arr[i] << i*8);
        }
        return res;
    }

    // Auxiliar variable to store value of an unsigned as a byte array.
    // This we avoid creating this temporary object every time we need one.
    static char _auxByteArray[sizeof(unsigned)];
};

char ByteArray::_auxByteArray[sizeof(unsigned)];

////////////////// Ugly hack to circumvent problem of having a fixed makefile
#include "RecordEncoder.h"
#include "RecordEncoder.cc"
#include "RecordDecoder.h"
#include "RecordDecoder.cc"
#include "Page.h"
#include "Page.cc"


RecordBasedFileManager* RecordBasedFileManager::_rbf_manager = 0;

RecordBasedFileManager* RecordBasedFileManager::instance() {
    if(!_rbf_manager)
        _rbf_manager = new RecordBasedFileManager();

    return _rbf_manager;
}

RecordBasedFileManager::RecordBasedFileManager() : 
    _pfm(*PagedFileManager::instance()) {
}

RecordBasedFileManager::~RecordBasedFileManager() {
}

RC RecordBasedFileManager::createFile(const string& fileName) {
    return _pfm.createFile(fileName);
}

RC RecordBasedFileManager::destroyFile(const string& fileName) {
    return _pfm.destroyFile(fileName);
}

RC RecordBasedFileManager::openFile(const string& fileName,
                                    FileHandle& fileHandle) {
    return _pfm.openFile(fileName, fileHandle);
}

RC RecordBasedFileManager::closeFile(FileHandle& fileHandle) {
    return _pfm.closeFile(fileHandle);
}

RC RecordBasedFileManager::insertRecord(
    FileHandle& fileHandle,
    const vector<Attribute>& recordDescriptor,
    const void *data,
    RID& rid) {


    RecordEncoder re((char *) data, recordDescriptor);

    int pageNum;
    Page *p = findPageToInsert(fileHandle, re.sizeAfterEncode(), &pageNum);
    if (p != NULL) {
        rid.pageNum = pageNum;
        rid.slotNum = p->insertRecord(re);
        delete p;
        return 0;
    }
    else {
        {
            Page p;
            fileHandle.appendPage(p.getData());
        }
        return insertRecord(fileHandle, recordDescriptor, data, rid);
    }

    return -1;
}

Page* RecordBasedFileManager::findPageToInsert(FileHandle& fileHandle,
                                               int sizeRecord,
                                               int *pageNum) {
    unsigned nPages = fileHandle.getNumberOfPages();
    if (nPages > 0) {
        Page *p = new Page();
        // Start with the last page
        for (int i = nPages-1; i >= 0; --i) {
            if (fileHandle.readPage(i, p->getData()) != -1) {
                if (p->canStoreRecord(sizeRecord)) {
                    *pageNum = i;
                    return p;
                }
            }
        }
    }

    // Either there are no pages yet, or all of them are full
    return NULL;
}

RC RecordBasedFileManager::readRecord(
    FileHandle& fileHandle,
    const vector<Attribute>& recordDescriptor,
    const RID& rid,
    void *data) {

    Page p;

    if (fileHandle.readPage(rid.pageNum, p.getData()) != -1) {
        char *recordAddr;
        unsigned recordSize;
        if (p.getRecord(rid.slotNum, &recordAddr, &recordSize) != -1) {
            RecordDecoder rd(recordAddr, recordSize, recordDescriptor);
            rd.decode((char *) data);
            return 0;
        }
    }

    return -1;
}

RC RecordBasedFileManager::printRecord(
    const vector<Attribute>& recordDescriptor,
    const void *data) {
    return -1;
}
